'use strict';

// ---------- configuration (BM25 constants must make sense with the C# indexer) ----------
// Offline help bundles (WebView2 app) inject window.DYNO_OFFLINE via package-offline:
//   { helpBase: "../", remote: "https://dynomotion.com/site/searchdata" }
// On the website DYNO_OFFLINE is undefined and none of the offline logic runs.
const OFFLINE = window.DYNO_OFFLINE || null;
let idxBase = 'searchdata';    // switched to OFFLINE.remote when the live index is reachable
let idxSource = 'bundled';
const K1 = 1.2, B = 0.75;
const TITLE_WEIGHT = 2.5;      // a title hit counts as this many body hits
const PHRASE_BONUS = 1.2;
const PAGE_SIZE = 10;
const MAX_EXPANSIONS = 100;    // cap for prefix* wildcards
const SECTION_NAMES = ['Help', 'Wiki', 'Forum', 'Store'];
const NUM_SECTIONS = 4;
const STORE_SECTION = 3;

let manifest = null;
const termShardCache = new Map();
const docShardCache = new Map();
let lastResults = null;        // {ranked:[[docId,score]...], terms:[...], query}

// ---------- tokenizer: must stay in sync with Tokenizer in the C# indexer ----------
function fold(s) {
  // The character class is the literal combining-diacritics range U+0300-U+036F.
  return s.toLowerCase().normalize("NFD").replace(/[̀-ͯ]/g, '');
}
function isTokenChar(c) {
  return (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9') || c === '_';
}
function isDigit(c) { return c >= '0' && c <= '9'; }
// Mirrors the C# indexer: a '.' BETWEEN DIGITS stays inside the token, so
// "5.4.4", "0.5", "192.168.0.1" are single terms.
function tokenize(s) {
  const f = fold(s), tokens = [];
  let i = 0;
  const n = f.length;
  while (i < n) {
    if (isTokenChar(f[i])) {
      const start = i;
      while (i < n) {
        if (isTokenChar(f[i])) { i++; continue; }
        if (f[i] === '.' && i > start && isDigit(f[i - 1]) && i + 1 < n && isDigit(f[i + 1])) { i++; continue; }
        break;
      }
      tokens.push(f.slice(start, i));
    } else i++;
  }
  return tokens;
}
function shardKey(t) {
  const m = c => (c >= 'a' && c <= 'z') ? c : (c >= '0' && c <= '9') ? '9' : '_';
  return m(t[0]) + (t.length > 1 ? m(t[1]) : '_');
}

// ---------- index loading ----------
async function fetchJson(url, init) {
  const r = await fetch(url, init);
  if (!r.ok) return null;
  return r.json();
}
async function fetchWithTimeout(url, ms, init) {
  const ctrl = new AbortController();
  const t = setTimeout(() => ctrl.abort(), ms);
  try { return await fetch(url, { ...init, signal: ctrl.signal }); }
  finally { clearTimeout(t); }
}

// ---- script-tag data loading ----
// Offline bundles ship the index as .js files (package-offline sets
// OFFLINE.scriptData) loaded via <script>, because fetch() of local files is
// blocked on file:// pages — script tags are not. Works over http(s) too.
const SCRIPT_DATA = !!(OFFLINE && OFFLINE.scriptData);
const scriptShardCache = new Map();
const scriptPending = {};
window.__dynoShard = (name, data) => {
  const resolve = scriptPending[name];
  if (resolve) { delete scriptPending[name]; resolve(data); }
};
function loadViaScript(name) {
  if (!scriptShardCache.has(name)) {
    scriptShardCache.set(name, new Promise(resolve => {
      scriptPending[name] = resolve;
      const s = document.createElement('script');
      s.src = `searchdata/${name}.js`;
      s.onerror = () => { delete scriptPending[name]; resolve(null); };
      document.head.appendChild(s);
    }));
  }
  return scriptShardCache.get(name);
}
async function ensureManifest() {
  if (manifest) return manifest;
  // Offline bundle: prefer the live website index when reachable, so search
  // stays current; fall back to the bundled snapshot when offline.
  if (OFFLINE && OFFLINE.remote) {
    try {
      const r = await fetchWithTimeout(`${OFFLINE.remote}/manifest.json`, 4000, { cache: 'no-cache' });
      if (r.ok) {
        manifest = await r.json();
        idxBase = OFFLINE.remote;
        idxSource = 'live';
      }
    } catch (e) { /* offline / blocked: use the bundled snapshot */ }
  }
  if (!manifest) {
    // no-cache: revalidate so a freshly uploaded index is picked up immediately;
    // the term/doc shards are versioned via ?v= and can cache long-term.
    manifest = SCRIPT_DATA
      ? await loadViaScript('manifest')
      : await fetchJson(`${idxBase}/manifest.json`, { cache: 'no-cache' });
  }
  if (!manifest) throw new SearchError('Search index not found — has it been built and uploaded?');
  const src = OFFLINE ? (idxSource === 'live' ? ' · live index' : ' · bundled index (offline)') : '';
  document.getElementById('buildinfo').textContent =
    `${manifest.docCount} pages indexed · index built ${fmtBuilt(manifest.built)}${src}`;
  return manifest;
}
function fmtBuilt(b) {
  return `${b.slice(0, 4)}-${b.slice(4, 6)}-${b.slice(6, 8)}`;
}
function loadTermShard(key) {
  if (!termShardCache.has(key)) {
    const p = (SCRIPT_DATA && idxSource === 'bundled')
      ? loadViaScript(`terms/t_${key}`).then(x => x || {})
      : fetchJson(`${idxBase}/terms/t_${key}.json?v=${manifest.built}`).then(x => x || {});
    termShardCache.set(key, p);
  }
  return termShardCache.get(key);
}
function loadDocShard(k) {
  if (!docShardCache.has(k)) {
    const p = (SCRIPT_DATA && idxSource === 'bundled')
      ? loadViaScript(`docs/d_${k}`).then(x => x || [])
      : fetchJson(`${idxBase}/docs/d_${k}.json?v=${manifest.built}`).then(x => x || []);
    docShardCache.set(k, p);
  }
  return docShardCache.get(k);
}

// Offline bundle + bundled snapshot in use: Help results point at the local
// help files shipped alongside (the snapshot was built from the same content).
// When the LIVE index is in use we keep online URLs — they match that index.
function localizeUrl(u) {
  if (!OFFLINE || idxSource === 'live') return u;
  const m = /^https:\/\/(?:www\.)?dynomotion\.com\/help\/(.+)$/i.exec(u);
  return m ? (OFFLINE.helpBase || '../') + m[1] : u;
}
async function getDoc(docId) {
  const shard = await loadDocShard(Math.floor(docId / manifest.docShardSize));
  return shard.find(d => d.i === docId);
}

class SearchError extends Error {}

// ---------- query language ----------
// grammar:  or := and ('OR' and)* ;  and := unary+ ;  unary := ('-'|'NOT') primary | primary
//           primary := '"phrase"' | word | word* | '(' or ')'
function lexQuery(q) {
  const out = [];
  let i = 0;
  while (i < q.length) {
    const c = q[i];
    if (/\s/.test(c)) { i++; continue; }
    if (c === '"') {
      const end = q.indexOf('"', i + 1);
      const inner = end < 0 ? q.slice(i + 1) : q.slice(i + 1, end);
      i = end < 0 ? q.length : end + 1;
      const words = tokenize(inner);
      if (words.length === 1) out.push({ type: 'TERM', term: words[0] });
      else if (words.length > 1) out.push({ type: 'PHRASE', words });
      continue;
    }
    if (c === '(') { out.push({ type: 'LP' }); i++; continue; }
    if (c === ')') { out.push({ type: 'RP' }); i++; continue; }
    if (c === '-') { out.push({ type: 'NOT' }); i++; continue; }
    let j = i;
    while (j < q.length && !/[\s()"]/.test(q[j])) j++;
    const raw = q.slice(i, j);
    i = j;
    const up = raw.toUpperCase();
    if (up === 'OR') { out.push({ type: 'OR' }); continue; }
    if (up === 'AND') continue;                       // AND is implicit
    if (up === 'NOT') { out.push({ type: 'NOT' }); continue; }
    const wildcard = raw.endsWith('*');
    const words = tokenize(wildcard ? raw.slice(0, -1) : raw);
    if (words.length === 0) continue;
    if (words.length === 1) out.push({ type: 'TERM', term: words[0], prefix: wildcard });
    else out.push({ type: 'PHRASE', words });         // e.g. "closed-loop" typed unquoted
  }
  return out;
}

function parseQuery(q) {
  const toks = lexQuery(q);
  let p = 0;
  function parseOr() {
    const parts = [parseAnd()];
    while (p < toks.length && toks[p].type === 'OR') { p++; parts.push(parseAnd()); }
    return parts.length === 1 ? parts[0] : { type: 'ORNODE', parts };
  }
  function parseAnd() {
    const pos = [], neg = [];
    while (p < toks.length && toks[p].type !== 'OR' && toks[p].type !== 'RP') {
      if (toks[p].type === 'NOT') {
        p++;
        const u = parsePrimary();
        if (u) neg.push(u);
      } else {
        const u = parsePrimary();
        if (u) pos.push(u);
      }
    }
    return { type: 'ANDNODE', pos, neg };
  }
  function parsePrimary() {
    const t = toks[p];
    if (!t) return null;
    if (t.type === 'LP') {
      p++;
      const inner = parseOr();
      if (toks[p] && toks[p].type === 'RP') p++;
      return inner;
    }
    if (t.type === 'TERM' || t.type === 'PHRASE') { p++; return t; }
    p++;                                              // stray operator/paren: skip
    return null;
  }
  const ast = parseOr();
  if (p < toks.length) p = toks.length;               // tolerate trailing ')'
  return ast;
}

// ---------- scoring ----------
function idf(df) {
  const N = manifest.docCount;
  return Math.log(1 + (N - df + 0.5) / (df + 0.5));
}
function bm25tf(tf, docLen) {
  return tf * (K1 + 1) / (tf + K1 * (1 - B + B * docLen / manifest.avgdl));
}
async function getPostings(term) {
  const shard = await loadTermShard(shardKey(term));
  return Object.hasOwn(shard, term) ? shard[term] : null;
}
function decodePositions(deltas) {
  let p = 0;
  return deltas.map(d => (p += d));
}

// Every eval returns {docs: Map(docId -> score), terms: [tokens used, for highlighting]}
async function evalNode(node) {
  if (node.type === 'TERM') return node.prefix ? evalPrefix(node.term) : evalTerm(node.term);
  if (node.type === 'PHRASE') return evalPhrase(node.words);
  if (node.type === 'ORNODE') {
    const results = await Promise.all(node.parts.map(evalNode));
    const docs = new Map(), terms = [];
    for (const r of results) {
      terms.push(...r.terms);
      for (const [d, s] of r.docs) docs.set(d, (docs.get(d) || 0) + s);
    }
    return { docs, terms };
  }
  if (node.type === 'ANDNODE') {
    if (node.pos.length === 0) {
      if (node.neg.length > 0)
        throw new SearchError('A query cannot be only exclusions — add at least one word to search for.');
      return { docs: new Map(), terms: [] };
    }
    const results = await Promise.all(node.pos.map(evalNode));
    results.sort((a, b) => a.docs.size - b.docs.size);
    let docs = new Map(results[0].docs);
    for (let i = 1; i < results.length; i++) {
      const next = new Map();
      for (const [d, s] of docs)
        if (results[i].docs.has(d)) next.set(d, s + results[i].docs.get(d));
      docs = next;
    }
    for (const r of await Promise.all(node.neg.map(evalNode)))
      for (const d of r.docs.keys()) docs.delete(d);
    return { docs, terms: results.flatMap(r => r.terms) };
  }
  return { docs: new Map(), terms: [] };
}

async function evalTerm(term) {
  const post = await getPostings(term);
  const docs = new Map();
  if (post) {
    const w = idf(post.length);
    for (const [doc, tfT, tfB] of post)
      docs.set(doc, w * bm25tf(tfB + TITLE_WEIGHT * tfT, manifest.doclens[doc]));
  }
  return { docs, terms: [term] };
}

async function evalPrefix(prefix) {
  if (prefix.length < 2)
    throw new SearchError(`Wildcard "${prefix}*" is too broad — use at least 2 characters before the *.`);
  const shard = await loadTermShard(shardKey(prefix));
  const docs = new Map();
  const used = [];
  for (const term of Object.keys(shard)) {
    if (!term.startsWith(prefix)) continue;
    if (used.length >= MAX_EXPANSIONS) break;
    used.push(term);
    const post = shard[term];
    const w = idf(post.length);
    for (const [doc, tfT, tfB] of post) {
      const s = w * bm25tf(tfB + TITLE_WEIGHT * tfT, manifest.doclens[doc]);
      if (s > (docs.get(doc) || 0)) docs.set(doc, s);   // dis-max across expansions
    }
  }
  return { docs, terms: used };
}

async function evalPhrase(words) {
  const posts = [];
  for (const w of words) {
    const p = await getPostings(w);
    if (!p) return { docs: new Map(), terms: words };
    posts.push(p);
  }
  const maps = posts.map(p => {
    const m = new Map();
    for (const [doc, , , deltas] of p) m.set(doc, decodePositions(deltas));
    return m;
  });
  const w = posts.reduce((s, p) => s + idf(p.length), 0);
  const docs = new Map();
  outer:
  for (const [doc, pos0] of maps[0]) {
    for (let k = 1; k < maps.length; k++) if (!maps[k].has(doc)) continue outer;
    const sets = maps.slice(1).map(m => new Set(m.get(doc)));
    let count = 0;
    for (const p0 of pos0) {
      let ok = true;
      for (let k = 0; k < sets.length; k++) if (!sets[k].has(p0 + k + 1)) { ok = false; break; }
      if (ok) count++;
    }
    if (count > 0)
      docs.set(doc, w * bm25tf(count, manifest.doclens[doc]) * PHRASE_BONUS);
  }
  return { docs, terms: words };
}

// ---------- snippets ----------
function escapeHtml(s) {
  return s.replace(/&/g, '&amp;').replace(/</g, '&lt;').replace(/>/g, '&gt;');
}

function highlight(text, termSet, maxLen) {
  // Tokenize with offsets and mark matching words; if maxLen given, pick the best window.
  // Scan the ORIGINAL text (offsets must match what we slice) and fold each
  // word only for comparison against the query terms. Same digit-dot rule as
  // tokenize() so terms like "5.4.4" highlight as one word.
  const words = [];
  const isWordChar = c => /[a-zA-Z0-9_À-ɏ]/.test(c);
  let i = 0;
  const n = text.length;
  while (i < n) {
    if (isWordChar(text[i])) {
      const start = i;
      while (i < n) {
        if (isWordChar(text[i])) { i++; continue; }
        if (text[i] === '.' && i > start && isDigit(text[i - 1]) && i + 1 < n && isDigit(text[i + 1])) { i++; continue; }
        break;
      }
      const w = fold(text.slice(start, i));
      words.push({ w, start, end: i, hit: termSet.has(w) });
    } else i++;
  }

  let from = 0, to = text.length, prefix = '', suffix = '';
  if (maxLen && text.length > maxLen) {
    let best = -1, bestScore = -1;
    for (let i = 0; i < words.length; i++) {
      if (!words[i].hit) continue;
      const seen = new Set();
      let total = 0;
      for (let j = i; j < Math.min(i + 30, words.length); j++)
        if (words[j].hit) { seen.add(words[j].w); total++; }
      const score = seen.size * 10 + total;
      if (score > bestScore) { bestScore = score; best = i; }
    }
    if (best < 0) {
      from = 0; to = maxLen;
    } else {
      from = words[Math.max(0, best - 8)].start;
      to = Math.min(from + maxLen, text.length);
    }
    if (from > 0) prefix = '… ';
    if (to < text.length) suffix = ' …';
  }

  let html = prefix, cursor = from;
  for (const wd of words) {
    if (!wd.hit || wd.end > to) continue;
    if (wd.start < cursor) continue;
    html += escapeHtml(text.slice(cursor, wd.start));
    html += '<mark>' + escapeHtml(text.slice(wd.start, wd.end)) + '</mark>';
    cursor = wd.end;
  }
  html += escapeHtml(text.slice(cursor, to)) + suffix;
  return html;
}

// ---------- search + rendering ----------
async function runSearch(query, page, pushHistory) {
  const stats = document.getElementById('stats');
  const resultsEl = document.getElementById('results');
  const pagerEl = document.getElementById('pager');

  const sections = new Set();
  for (let s = 0; s < NUM_SECTIONS; s++)
    if (document.getElementById('sec' + s).checked) sections.add(s);

  if (!query.trim()) {
    stats.textContent = '';
    resultsEl.innerHTML = '';
    pagerEl.innerHTML = '';
    return;
  }

  try {
    stats.textContent = 'Searching…';
    await ensureManifest();
    const t0 = performance.now();
    const { docs, terms } = await evalNode(parseQuery(query));

    const ranked = [];
    const perSection = [0, 0, 0, 0];
    for (const [d, s] of docs) {
      const sec = manifest.sections[d];
      perSection[sec]++;
      if (sections.has(sec)) ranked.push([d, s]);
    }
    ranked.sort((a, b) => b[1] - a[1]);

    // Optional-chained: tolerate a stale cached search.html that lacks #sortsel.
    const order = document.getElementById('sortsel')?.value || 'rel';
    const dates = manifest.dates || [];
    if (order === 'new') {
      // Undated pages (0) sink to the bottom; ties break by relevance.
      ranked.sort((a, b) => (dates[b[0]] || 0) - (dates[a[0]] || 0) || b[1] - a[1]);
    } else if (order === 'old') {
      ranked.sort((a, b) => {
        const da = dates[a[0]] || 0, db = dates[b[0]] || 0;
        if (!da && !db) return b[1] - a[1];
        if (!da) return 1;
        if (!db) return -1;
        return da - db || b[1] - a[1];
      });
    } else {
      // Product-name pinning applies to relevance order only: if the whole query
      // is a product name, the store's purchase page goes first (unless the
      // Store section is un-checked).
      const products = manifest.products || {};
      const qkey = tokenize(query).join(' ');
      if (sections.has(STORE_SECTION) && Object.hasOwn(products, qkey)) {
        const pid = products[qkey];
        const at = ranked.findIndex(r => r[0] === pid);
        if (at > 0) ranked.unshift(ranked.splice(at, 1)[0]);
        else if (at === -1) ranked.unshift([pid, 0]);
      }
    }
    lastResults = { ranked, terms: new Set(terms), query };

    const ms = Math.max(1, Math.round(performance.now() - t0));
    const parts = perSection.map((n, i) => `${n} ${SECTION_NAMES[i].toLowerCase()}`).join(', ');
    stats.textContent = ranked.length === 0
      ? `No results (${parts}). Tip: try a wildcard like ${firstWord(query)}* or OR between alternatives.`
      : `${ranked.length} result${ranked.length === 1 ? '' : 's'} (${parts}) — ${ms} ms`;

    if (pushHistory) {
      const u = new URL(location.href);
      u.searchParams.set('q', query);
      u.searchParams.set('s', [...sections].join(''));
      if (order === 'rel') u.searchParams.delete('o');
      else u.searchParams.set('o', order);
      try { history.pushState(null, '', u); } catch (e) { /* some file:// contexts refuse */ }
    }
    await renderPage(page);
  } catch (e) {
    resultsEl.innerHTML = '';
    pagerEl.innerHTML = '';
    stats.innerHTML = `<span class="err">${escapeHtml(e instanceof SearchError ? e.message : 'Search failed: ' + e.message)}</span>`;
  }
}

const MONTH_NAMES = ['Jan', 'Feb', 'Mar', 'Apr', 'May', 'Jun', 'Jul', 'Aug', 'Sep', 'Oct', 'Nov', 'Dec'];
function fmtDocDate(d) {
  if (!d) return '';
  const y = Math.floor(d / 10000), m = Math.floor(d / 100) % 100, day = d % 100;
  if (m < 1 || m > 12) return '';
  return `<span class="rdate">${MONTH_NAMES[m - 1]} ${day}, ${y} — </span>`;
}

function firstWord(q) {
  const t = tokenize(q);
  return t.length ? t[0] : 'word';
}

async function renderPage(page) {
  const { ranked, terms } = lastResults;
  const resultsEl = document.getElementById('results');
  const pagerEl = document.getElementById('pager');
  const pageCount = Math.ceil(ranked.length / PAGE_SIZE);
  page = Math.min(Math.max(0, page), Math.max(0, pageCount - 1));

  const slice = ranked.slice(page * PAGE_SIZE, (page + 1) * PAGE_SIZE);
  const docsOnPage = await Promise.all(slice.map(([d]) => getDoc(d)));

  let html = '';
  for (let i = 0; i < slice.length; i++) {
    const doc = docsOnPage[i];
    if (!doc) continue;
    const titleHtml = highlight(doc.t, terms, 0);
    const snippetHtml = highlight(doc.b, terms, 300);
    const dateHtml = fmtDocDate(manifest.dates && manifest.dates[doc.i]);
    const link = localizeUrl(doc.u);
    html += `<div class="result">
      <p class="rtitle"><span class="badge s${doc.s}">${SECTION_NAMES[doc.s]}</span><a href="${escapeHtml(link)}">${titleHtml}</a></p>
      <p class="rurl">${escapeHtml(link)}</p>
      <p class="rsnip">${dateHtml}${snippetHtml}</p>
    </div>`;
  }
  resultsEl.innerHTML = html;

  let pager = '';
  if (pageCount > 1) {
    pager += `<button ${page === 0 ? 'disabled' : ''} data-page="${page - 1}">‹ Prev</button>`;
    const lo = Math.max(0, page - 3), hi = Math.min(pageCount, page + 4);
    for (let p = lo; p < hi; p++)
      pager += `<button class="${p === page ? 'cur' : ''}" data-page="${p}">${p + 1}</button>`;
    pager += `<button ${page >= pageCount - 1 ? 'disabled' : ''} data-page="${page + 1}">Next ›</button>`;
  }
  pagerEl.innerHTML = pager;
  for (const btn of pagerEl.querySelectorAll('button[data-page]')) {
    btn.addEventListener('click', () => {
      renderPage(parseInt(btn.dataset.page, 10));
      window.scrollTo({ top: 0 });
    });
  }
}

// ---------- wiring ----------
document.getElementById('searchform').addEventListener('submit', e => {
  e.preventDefault();
  runSearch(document.getElementById('q').value, 0, true);
});
for (let s = 0; s < NUM_SECTIONS; s++) {
  document.getElementById('sec' + s).addEventListener('change', () => {
    const q = (lastResults && lastResults.query) || document.getElementById('q').value;
    if (q.trim()) runSearch(q, 0, true);
  });
}
document.getElementById('sortsel')?.addEventListener('change', () => {
  // Re-order what's on screen: use the last SUBMITTED query, not the input box,
  // which the user may have edited without searching.
  const q = (lastResults && lastResults.query) || document.getElementById('q').value;
  if (q.trim()) runSearch(q, 0, true);
});
window.addEventListener('popstate', initFromUrl);

function initFromUrl() {
  const u = new URL(location.href);
  const q = u.searchParams.get('q') || '';
  // Accept both ?s=0123 (our own links) and repeated ?s=0&s=1... (plain HTML
  // checkbox forms, e.g. the Shopify header). Absent = all sections.
  const parts = u.searchParams.getAll('s');
  if (parts.length > 0) {
    const s = parts.join('');
    for (let i = 0; i < NUM_SECTIONS; i++)
      document.getElementById('sec' + i).checked = s.includes(String(i));
  }
  const o = u.searchParams.get('o');
  const sel = document.getElementById('sortsel');
  if (sel) sel.value = (o === 'new' || o === 'old') ? o : 'rel';
  document.getElementById('q').value = q;
  if (q) runSearch(q, 0, false);
  else {
    // Back-navigation to a query-less entry: don't leave stale results rendered.
    lastResults = null;
    document.getElementById('stats').textContent = '';
    document.getElementById('results').innerHTML = '';
    document.getElementById('pager').innerHTML = '';
  }
  if (location.hash === '#syntax') {
    const d = document.getElementById('syntax');
    if (d) { d.open = true; d.scrollIntoView(); }
  }
}

if (OFFLINE)  // header "Help Manuals" goes to the local help root in the app
  document.getElementById('nav-help')?.setAttribute('href', (OFFLINE.helpBase || '../') + 'index.htm');

ensureManifest().then(() => {
  // Offline bundle opened directly (no query or sections in the URL) while the
  // bundled snapshot is in use: pre-select Manuals only, since wiki/forum/store
  // results link to the internet. Explicit URLs and searches are left alone.
  if (OFFLINE && idxSource === 'bundled') {
    const u = new URL(location.href);
    if (!u.searchParams.get('q') && u.searchParams.getAll('s').length === 0)
      for (let i = 1; i < NUM_SECTIONS; i++) document.getElementById('sec' + i).checked = false;
  }
}).catch(() => { /* surfaced on first search */ });
initFromUrl();
