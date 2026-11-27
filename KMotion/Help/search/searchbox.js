// Dynomotion help-page search bar (offline-capable twin of /Help/global-search.js).
// Injected into every Help page as:
//   <script src="[../]search/searchbox.js" defer></script>
// Renders a search form (replacing the old Google CSE in #searchNav when present)
// that submits to the bundled search page next to this script — or to the
// website search when the page is being viewed on dynomotion.com.
//
// Search contract: q = query; s = section (repeatable: 0 Manuals, 1 Wiki,
// 2 Forum, 3 Store; none sent = all); o = sort (rel | new | old).
//
// NOTE: keep this file pure ASCII — help pages are windows-1252 and scripts
// inherit the page charset, so UTF-8 punctuation renders as mojibake.
(function () {
  'use strict';
  var cs = document.currentScript;
  if (!cs || !cs.src) return;
  var base = cs.src.slice(0, cs.src.lastIndexOf('/') + 1);       // .../Help/search/
  var onSite = /(^|\.)dynomotion\.com$/i.test(location.hostname);
  var searchPage = onSite ? 'https://dynomotion.com/site/search.html'
                          : base + 'search.html';

  // Instant offline hint (no network probe, no delay): when the OS reports no
  // connectivity, pre-check only Manuals — wiki/forum/store links need internet.
  // The search page still makes the authoritative live-vs-bundled decision.
  var offlineNow = (navigator.onLine === false);

  var css = document.createElement('style');
  css.textContent =
    '#dynoSearchBar{font-family:"Segoe UI",Arial,sans-serif;margin:4px 0;width:365px;max-width:72vw;}' +
    '#dynoSearchBar .dsRow{display:flex;gap:6px;}' +
    '#dynoSearchBar input[type=search]{flex:1;min-width:120px;padding:5px 10px;border:1px solid #bbb;' +
      'border-radius:6px;font-size:13px;}' +
    '#dynoSearchBar input[type=search]:focus{outline:none;border-color:#e89c0a;}' +
    '#dynoSearchBar button{padding:5px 14px;border:none;border-radius:6px;background:#e89c0a;' +
      'color:#1a1a1a;font-weight:600;font-size:13px;cursor:pointer;}' +
    '#dynoSearchBar button:hover{background:#c78508;}' +
    '#dynoSearchBar .dsOpts{display:flex;flex-wrap:wrap;gap:2px 9px;align-items:center;' +
      'margin-top:3px;font-size:11.5px;color:#444;}' +
    '#dynoSearchBar .dsOpts label{display:inline-flex;align-items:center;gap:3px;cursor:pointer;' +
      'margin:0;font-weight:normal;white-space:nowrap;}' +
    '#dynoSearchBar .dsOpts select{font-size:11.5px;border:1px solid #ccc;border-radius:4px;padding:1px 2px;}' +
    '#dynoSearchBar .dsOpts a{margin-left:auto;color:#777;white-space:nowrap;}';
  (document.head || document.documentElement).appendChild(css);

  var form = document.createElement('form');
  form.id = 'dynoSearchBar';
  form.setAttribute('action', searchPage);
  form.setAttribute('method', 'get');
  form.setAttribute('role', 'search');

  var row = document.createElement('div');
  row.className = 'dsRow';
  var input = document.createElement('input');
  input.type = 'search';
  input.name = 'q';
  input.placeholder = 'Search docs, forum, wiki & store...';
  input.setAttribute('aria-label', 'Search all of Dynomotion');
  var btn = document.createElement('button');
  btn.type = 'submit';
  btn.textContent = 'Search';
  row.appendChild(input);
  row.appendChild(btn);
  form.appendChild(row);

  var opts = document.createElement('div');
  opts.className = 'dsOpts';
  var sections = [['0', 'Manuals'], ['1', 'Wiki'], ['2', 'Forum'], ['3', 'Store']];
  for (var i = 0; i < sections.length; i++) {
    var lab = document.createElement('label');
    var cb = document.createElement('input');
    cb.type = 'checkbox';
    cb.name = 's';
    cb.value = sections[i][0];
    cb.checked = sections[i][0] === '0' ? true : !offlineNow;
    lab.appendChild(cb);
    lab.appendChild(document.createTextNode(' ' + sections[i][1]));
    opts.appendChild(lab);
  }
  var sortLab = document.createElement('label');
  sortLab.appendChild(document.createTextNode('Sort: '));
  var sortSel = document.createElement('select');
  sortSel.name = 'o';
  var sortOptions = [['rel', 'Relevance'], ['new', 'Newest'], ['old', 'Oldest']];
  for (var j = 0; j < sortOptions.length; j++) {
    var op = document.createElement('option');
    op.value = sortOptions[j][0];
    op.textContent = sortOptions[j][1];
    sortSel.appendChild(op);
  }
  sortLab.appendChild(sortSel);
  opts.appendChild(sortLab);
  var tips = document.createElement('a');
  tips.href = searchPage + '#syntax';
  tips.textContent = 'Search tips';
  opts.appendChild(tips);
  form.appendChild(opts);

  form.addEventListener('submit', function (e) {
    if (!input.value.trim()) { e.preventDefault(); return; }
    sortSel.disabled = (sortSel.value === 'rel');   // keep default sort out of the URL
  });

  var host = document.getElementById('searchNav');
  if (host) {
    host.innerHTML = '';        // retires the old Google CSE box
    // The old CSE container was positioned hanging off-screen (right:-59px),
    // and some pages also pin `left`, which would stretch the box and clip us.
    host.style.left = 'auto';
    host.style.right = '10px';
    host.style.width = 'auto';
    host.appendChild(form);
  } else if (document.body) {
    document.body.insertBefore(form, document.body.firstChild);
  }

  // The two-row bar is taller than the old CSE box: nudge the Google Translate
  // widget down if it would sit under/over the form.
  var gt = document.getElementById('google_translate_element');
  if (gt) {
    var fb = form.getBoundingClientRect();
    var gb = gt.getBoundingClientRect();
    var overlap = fb.bottom + 6 - gb.top;
    var horizontal = fb.left < gb.right && gb.left < fb.right;
    if (overlap > 0 && horizontal) {
      var gcs = window.getComputedStyle(gt);
      if (gcs.position === 'absolute') gt.style.top = ((parseFloat(gcs.top) || 0) + overlap) + 'px';
      else gt.style.marginTop = overlap + 'px';
    }
  }
})();
