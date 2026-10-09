using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Windows.Forms;

namespace Migrate
{
    public sealed class MigrationForm : Form
    {
        private readonly TextBox destinationPath = PathBox();
        private readonly TextBox sourcePath = PathBox();
        private readonly ComboBox candidates = new ComboBox();
        private readonly Button browseDestination = MakeButton("Browse...");
        private readonly Button browseSource = MakeButton("Browse...");
        private readonly Button preview = MakeButton("Preview merge");
        private readonly Button dataDetails = MakeButton("View Data files");
        private readonly Button selectAll = MakeButton("Select all");
        private readonly Button selectNone = MakeButton("Select none");
        private readonly Button merge = MakeButton("Merge settings");
        private readonly Button close = MakeButton("Close");
        private readonly Label summary = new Label();
        private readonly Label status = new Label();
        private readonly ListView optionalFiles = new ListView();
        private readonly TextBox details = new TextBox();
        private readonly ProgressBar progress = new ProgressBar();
        private MigrationPlan plan;
        private bool busy;
        private bool copying;
        private bool loadingCandidates;
        private bool updatingOptionalFiles;
        private bool optionalSelectionUpdatePending;

        public MigrationForm(string source, string destination)
        {
            Text = "KMotion - Migrate settings";
            StartPosition = FormStartPosition.CenterScreen;
            ClientSize = new Size(960, 790);
            MinimumSize = new Size(840, 720);
            AutoScaleMode = AutoScaleMode.Font;
            Font = new Font("Segoe UI", 9F);
            BuildControls();
            sourcePath.Text = source ?? String.Empty;
            destinationPath.Text = destination ?? String.Empty;
            browseDestination.Click += delegate { BrowseInstallation(true); };
            browseSource.Click += delegate { BrowseInstallation(false); };
            candidates.SelectedIndexChanged += delegate
            {
                if (loadingCandidates || candidates.SelectedIndex < 0) return;
                InstallationCandidate candidate = candidates.SelectedItem as InstallationCandidate;
                if (candidate != null)
                {
                    sourcePath.Text = candidate.Root;
                    InvalidatePlan();
                }
            };
            preview.Click += delegate { PreviewMerge(); };
            dataDetails.Click += delegate { ShowDataFiles(); };
            selectAll.Click += delegate { SetChecks(true); };
            selectNone.Click += delegate { SetChecks(false); };
            optionalFiles.ItemChecked += delegate { QueueSelectionRefresh(); };
            merge.Click += delegate { MergeSettings(); };
            close.Click += delegate { Close(); };
            FormClosing += delegate(object sender, FormClosingEventArgs e)
            {
                // Do not interrupt a merge between writing a backup and its destination file.
                if (copying)
                {
                    e.Cancel = true;
                    status.Text = "Please wait until the merge finishes before closing this window.";
                }
            };
            Shown += delegate { FindInstallations(); };
            InvalidatePlan();
        }

        private static TextBox PathBox()
        {
            return new TextBox { ReadOnly = true, Dock = DockStyle.Fill, BackColor = SystemColors.Window, Margin = new Padding(0, 5, 8, 3) };
        }

        private static Button MakeButton(string text)
        {
            return new Button { Text = text, AutoSize = true, MinimumSize = new Size(88, 28), Margin = new Padding(0, 2, 8, 2) };
        }

        private static Label TextLabel(string text)
        {
            return new Label { Text = text, Dock = DockStyle.Fill, TextAlign = ContentAlignment.MiddleLeft, AutoSize = false };
        }

        private void BuildControls()
        {
            TableLayoutPanel page = new TableLayoutPanel { Dock = DockStyle.Fill, Padding = new Padding(18, 12, 18, 12), ColumnCount = 1, RowCount = 11 };
            page.ColumnStyles.Add(new ColumnStyle(SizeType.Percent, 100));
            page.RowStyles.Add(new RowStyle(SizeType.Absolute, 40));
            page.RowStyles.Add(new RowStyle(SizeType.Absolute, 52));
            page.RowStyles.Add(new RowStyle(SizeType.Absolute, 111));
            page.RowStyles.Add(new RowStyle(SizeType.Absolute, 39));
            page.RowStyles.Add(new RowStyle(SizeType.Absolute, 61));
            page.RowStyles.Add(new RowStyle(SizeType.Absolute, 33));
            page.RowStyles.Add(new RowStyle(SizeType.Percent, 100));
            page.RowStyles.Add(new RowStyle(SizeType.Absolute, 23));
            page.RowStyles.Add(new RowStyle(SizeType.Absolute, 97));
            page.RowStyles.Add(new RowStyle(SizeType.Absolute, 27));
            page.RowStyles.Add(new RowStyle(SizeType.Absolute, 38));
            Label heading = TextLabel("Would you like to merge settings from a previous version?");
            heading.Font = new Font(Font.FontFamily, 14F, FontStyle.Bold);
            page.Controls.Add(heading, 0, 0);
            page.Controls.Add(TextLabel("Choose the new and previous installations, then preview the merge. Close KMotion and KMotionCNC before merging.\r\nNothing is copied until you select Merge settings and confirm. You can close this window to skip migration."), 0, 1);

            TableLayoutPanel folders = new TableLayoutPanel { Dock = DockStyle.Fill, ColumnCount = 3, RowCount = 3, Margin = Padding.Empty };
            folders.ColumnStyles.Add(new ColumnStyle(SizeType.Absolute, 152));
            folders.ColumnStyles.Add(new ColumnStyle(SizeType.Percent, 100));
            folders.ColumnStyles.Add(new ColumnStyle(SizeType.Absolute, 105));
            for (int i = 0; i < 3; i++) folders.RowStyles.Add(new RowStyle(SizeType.Absolute, 36));
            folders.Controls.Add(TextLabel("New installation:"), 0, 0);
            folders.Controls.Add(destinationPath, 1, 0);
            folders.Controls.Add(browseDestination, 2, 0);
            folders.Controls.Add(TextLabel("Previous installation:"), 0, 1);
            folders.Controls.Add(sourcePath, 1, 1);
            folders.Controls.Add(browseSource, 2, 1);
            folders.Controls.Add(TextLabel("Detected versions:"), 0, 2);
            candidates.DropDownStyle = ComboBoxStyle.DropDownList;
            candidates.DisplayMember = "DisplayName";
            candidates.Dock = DockStyle.Fill;
            candidates.Margin = new Padding(0, 5, 8, 3);
            folders.Controls.Add(candidates, 1, 2);
            folders.SetColumnSpan(candidates, 2);
            page.Controls.Add(folders, 0, 2);

            FlowLayoutPanel previewBar = new FlowLayoutPanel { Dock = DockStyle.Fill, WrapContents = false, Margin = Padding.Empty };
            previewBar.Controls.Add(preview);
            previewBar.Controls.Add(dataDetails);
            page.Controls.Add(previewBar, 0, 3);
            summary.Dock = DockStyle.Fill;
            summary.BorderStyle = BorderStyle.FixedSingle;
            summary.Padding = new Padding(8, 5, 8, 5);
            page.Controls.Add(summary, 0, 4);

            TableLayoutPanel optionalBar = new TableLayoutPanel { Dock = DockStyle.Fill, ColumnCount = 3, RowCount = 1, Margin = Padding.Empty };
            optionalBar.ColumnStyles.Add(new ColumnStyle(SizeType.Percent, 100));
            optionalBar.ColumnStyles.Add(new ColumnStyle(SizeType.Absolute, 102));
            optionalBar.ColumnStyles.Add(new ColumnStyle(SizeType.Absolute, 102));
            optionalBar.Controls.Add(TextLabel("New or different referenced files (check to include):"), 0, 0);
            optionalBar.Controls.Add(selectAll, 1, 0);
            optionalBar.Controls.Add(selectNone, 2, 0);
            page.Controls.Add(optionalBar, 0, 5);
            optionalFiles.Dock = DockStyle.Fill;
            optionalFiles.View = View.Details;
            optionalFiles.CheckBoxes = true;
            optionalFiles.FullRowSelect = true;
            optionalFiles.HideSelection = false;
            optionalFiles.ShowItemToolTips = true;
            optionalFiles.Columns.Add("File relative to installation", 360);
            optionalFiles.Columns.Add("Action", 165);
            optionalFiles.Columns.Add("Referenced by", 335);
            page.Controls.Add(optionalFiles, 0, 6);
            page.Controls.Add(TextLabel("Warnings and details:"), 0, 7);
            details.Dock = DockStyle.Fill;
            details.Multiline = true;
            details.ReadOnly = true;
            details.ScrollBars = ScrollBars.Both;
            details.WordWrap = false;
            details.BackColor = SystemColors.Window;
            page.Controls.Add(details, 0, 8);
            status.Dock = DockStyle.Fill;
            status.AutoEllipsis = true;
            status.TextAlign = ContentAlignment.MiddleLeft;
            page.Controls.Add(status, 0, 9);

            TableLayoutPanel footer = new TableLayoutPanel { Dock = DockStyle.Fill, ColumnCount = 3, RowCount = 1, Margin = Padding.Empty };
            footer.ColumnStyles.Add(new ColumnStyle(SizeType.Percent, 100));
            footer.ColumnStyles.Add(new ColumnStyle(SizeType.Absolute, 142));
            footer.ColumnStyles.Add(new ColumnStyle(SizeType.Absolute, 100));
            progress.Dock = DockStyle.Fill;
            progress.Margin = new Padding(0, 8, 16, 8);
            progress.Visible = false;
            footer.Controls.Add(progress, 0, 0);
            merge.MinimumSize = new Size(130, 30);
            footer.Controls.Add(merge, 1, 0);
            footer.Controls.Add(close, 2, 0);
            page.Controls.Add(footer, 0, 10);
            Controls.Add(page);
            CancelButton = close;
        }

        private void BrowseInstallation(bool destination)
        {
            using (FolderBrowserDialog dialog = new FolderBrowserDialog())
            {
                dialog.Description = destination ? "Select the NEW KMotion installation folder." : "Select the PREVIOUS KMotion installation folder containing KMotion\\Data.";
                dialog.ShowNewFolderButton = false;
                dialog.SelectedPath = destination ? destinationPath.Text : sourcePath.Text;
                if (dialog.ShowDialog(this) != DialogResult.OK) return;
                try
                {
                    string root = InstallationPaths.NormalizeRoot(dialog.SelectedPath);
                    if (destination) destinationPath.Text = root;
                    else
                    {
                        sourcePath.Text = root;
                        loadingCandidates = true;
                        candidates.SelectedIndex = -1;
                        loadingCandidates = false;
                    }
                    InvalidatePlan();
                    if (destination) FindInstallations();
                }
                catch (Exception ex) { ShowError("Unable to select installation", ex.Message); }
            }
        }

        private void FindInstallations()
        {
            string destination = destinationPath.Text;
            StartWork(delegate(BackgroundWorker worker) { return InstallationDiscovery.Find(destination); },
                "Searching C:\\KMotion version folders...", false, delegate(object value)
                {
                    List<InstallationCandidate> found = (List<InstallationCandidate>)value;
                    loadingCandidates = true;
                    candidates.Items.Clear();
                    foreach (InstallationCandidate candidate in found) candidates.Items.Add(candidate);
                    if (String.IsNullOrWhiteSpace(sourcePath.Text) && found.Count > 0)
                    {
                        sourcePath.Text = found[0].Root;
                        candidates.SelectedIndex = 0;
                    }
                    else
                    {
                        for (int i = 0; i < found.Count; i++)
                            if (String.Equals(found[i].Root, sourcePath.Text, StringComparison.OrdinalIgnoreCase)) candidates.SelectedIndex = i;
                    }
                    loadingCandidates = false;
                    InvalidatePlan();
                    status.Text = found.Count == 0 ? "No previous installation was detected. Use Browse to select one." :
                        "Detected " + found.Count + " installation(s), with the highest version listed first.";
                });
        }

        private void InvalidatePlan()
        {
            plan = null;
            optionalFiles.Items.Clear();
            details.Clear();
            summary.Text = "Preview required. Only new or different files are shown and copied, comparing contents rather than dates.\r\nData is checked recursively, including path updates. Replaced files are backed up; destination-only files are kept.";
            status.Text = String.IsNullOrWhiteSpace(destinationPath.Text) ? "Select the new installation to continue." : "Select the previous installation, then preview the merge.";
            UpdateButtons();
        }

        private void UpdateButtons()
        {
            browseDestination.Enabled = !busy;
            browseSource.Enabled = !busy;
            candidates.Enabled = !busy && candidates.Items.Count > 0;
            preview.Enabled = !busy && !String.IsNullOrWhiteSpace(sourcePath.Text) && !String.IsNullOrWhiteSpace(destinationPath.Text);
            dataDetails.Enabled = !busy && plan != null && plan.DataFiles.Count > 0;
            merge.Enabled = !busy && plan != null && (plan.DataFiles.Count > 0 || optionalFiles.CheckedItems.Count > 0);
            optionalFiles.Enabled = !busy && plan != null;
            selectAll.Enabled = selectNone.Enabled = !busy && optionalFiles.Items.Count > 0;
            close.Enabled = !copying;
            ControlBox = !copying;
        }

        private void PreviewMerge()
        {
            string source = sourcePath.Text;
            string destination = destinationPath.Text;
            InvalidatePlan();
            StartWork(delegate(BackgroundWorker worker) { return MigrationEngine.BuildPlan(source, destination); },
                "Reading settings and referenced files...", false, delegate(object value)
                {
                    plan = (MigrationPlan)value;
                    RefreshPreviewSummary();
                });
        }

        private void PopulateOptionalFiles(IEnumerable<MigrationFile> selectedFiles)
        {
            HashSet<MigrationFile> selected = new HashSet<MigrationFile>(selectedFiles);
            updatingOptionalFiles = true;
            optionalFiles.BeginUpdate();
            try
            {
                optionalFiles.Items.Clear();
                foreach (MigrationFile file in plan.OptionalFiles)
                {
                    ListViewItem item = new ListViewItem(file.RelativePath);
                    string action = file.Overwrite ? "Back up and replace" : "Copy new file";
                    item.SubItems.Add(action);
                    item.SubItems.Add(file.Reason);
                    item.Tag = file;
                    item.Checked = selected.Contains(file);
                    item.ToolTipText = action + "\r\nFrom: " + file.SourcePath + "\r\nTo: " + file.DestinationPath;
                    optionalFiles.Items.Add(item);
                }
            }
            finally
            {
                optionalFiles.EndUpdate();
                updatingOptionalFiles = false;
            }
        }

        private List<MigrationFile> SelectedOptionalFiles()
        {
            List<MigrationFile> selected = new List<MigrationFile>();
            foreach (ListViewItem item in optionalFiles.CheckedItems) selected.Add((MigrationFile)item.Tag);
            return selected;
        }

        private void QueueSelectionRefresh()
        {
            if (updatingOptionalFiles || optionalSelectionUpdatePending || busy || plan == null) return;
            optionalSelectionUpdatePending = true;
            // ItemChecked runs after the check state changes. Coalesce Select all/none
            // and evaluate the current selection once on the next UI message.
            BeginInvoke((MethodInvoker)delegate
            {
                optionalSelectionUpdatePending = false;
                if (!IsDisposed && !busy && plan != null) RefreshPreviewSummary();
            });
        }

        private void RefreshPreviewSummary()
        {
            if (plan == null) return;
            List<MigrationFile> selected = SelectedOptionalFiles();
            try { MigrationEngine.RefreshDataFiles(plan, selected); }
            catch (Exception ex)
            {
                InvalidatePlan();
                details.Text = ex.Message;
                status.Text = "Unable to update the preview. Preview again before merging.";
                ShowError("Unable to update preview", ex.Message);
                return;
            }
            // Selecting a dependency can also make its referencing file identical.
            // Preserve checks only for rows that remain different after comparison.
            PopulateOptionalFiles(selected);
            selected = SelectedOptionalFiles();
            if (plan.DataFiles.Count == 0 && plan.OptionalFiles.Count == 0)
            {
                summary.Text = "No different files to migrate. Compared file contents already match the new installation.\r\nNo files will be copied and no backup will be created.";
                status.Text = "No different files to migrate. " + plan.Warnings.Count + " warning(s). No files have been copied.";
            }
            else
            {
                summary.Text = "Data: " + plan.DataFiles.Count + " new or different file(s), including " + CountOverwrites(plan.DataFiles) + " to back up and replace.\r\n" +
                    "Referenced files: " + plan.OptionalFiles.Count + " available; " + selected.Count + " selected, including " + CountOverwrites(selected) + " to back up and replace.";
                status.Text = plan.DataFiles.Count == 0 && selected.Count == 0 ?
                    "No different Data files. Check referenced files to include them. " + plan.Warnings.Count + " warning(s)." :
                    "Preview ready. " + plan.Warnings.Count + " warning(s). No files have been copied.";
            }
            details.Text = "Only new or different contents are shown, including recognized path updates; file dates do not affect the comparison." + Environment.NewLine +
                "Unchecked missing references can still use the previous installation; unchecked existing files in the new installation are kept." + Environment.NewLine +
                (plan.Warnings.Count == 0 ? "No warnings. Review the Data file list and select any referenced files before merging." :
                String.Join(Environment.NewLine, plan.Warnings.ToArray()));
            UpdateButtons();
        }

        private static int CountOverwrites(IEnumerable<MigrationFile> files)
        {
            int count = 0;
            foreach (MigrationFile file in files) if (file.Overwrite) count++;
            return count;
        }

        private void SetChecks(bool value)
        {
            updatingOptionalFiles = true;
            try { foreach (ListViewItem item in optionalFiles.Items) item.Checked = value; }
            finally { updatingOptionalFiles = false; }
            QueueSelectionRefresh();
        }

        private void MergeSettings()
        {
            if (plan == null || busy) return;
            // A click can arrive before the coalesced checkbox refresh message.
            RefreshPreviewSummary();
            if (plan == null) return;
            List<MigrationFile> selected = SelectedOptionalFiles();
            if (plan.DataFiles.Count == 0 && selected.Count == 0) return;
            string question = "Merge from:\r\n" + plan.SourceRoot + "\r\n\r\nInto:\r\n" + plan.DestinationRoot + "\r\n\r\n" +
                "Copy " + plan.DataFiles.Count + " new or different Data file(s), replacing " + CountOverwrites(plan.DataFiles) + " existing file(s), and " +
                selected.Count + " selected referenced file(s), replacing " + CountOverwrites(selected) + " existing file(s)?\r\n\r\n" +
                "Identical contents are skipped. Existing files will be backed up before replacement. Recognized references are updated to the new installation when their targets are available there; unchecked missing references can still use the previous installation.\r\n\r\n" +
                "Close KMotion and KMotionCNC before continuing.";
            if (plan.Warnings.Count > 0) question += "\r\n\r\nThe preview contains " + plan.Warnings.Count + " warning(s). Review them before continuing.";
            if (MessageBox.Show(this, question, "Confirm settings merge", MessageBoxButtons.YesNo, MessageBoxIcon.Question,
                MessageBoxDefaultButton.Button2) != DialogResult.Yes) return;
            MigrationPlan approvedPlan = plan;
            StartWork(delegate(BackgroundWorker worker)
                {
                    return MigrationEngine.Execute(approvedPlan, selected, delegate(string message) { worker.ReportProgress(0, message); });
                }, "Merging settings...", true, delegate(object value)
                {
                    MigrationResult result = (MigrationResult)value;
                    Environment.ExitCode = 0;
                    plan = null; // A repeat merge must build and approve a fresh preview.
                    optionalFiles.Items.Clear();
                    string outcome = result.FilesCopied == 0 ? "No different files to migrate. No files were copied." : "Merge complete: " + result.FilesCopied + " file(s) copied.";
                    string backupDetails = String.IsNullOrEmpty(result.BackupDirectory) ? "No backup was created." : "Backup and log: " + result.BackupDirectory;
                    summary.Text = outcome + "\r\n" + backupDetails;
                    details.Text = backupDetails + Environment.NewLine +
                        (result.Warnings.Count == 0 ? "No warnings." : String.Join(Environment.NewLine, result.Warnings.ToArray()));
                    status.Text = result.FilesCopied == 0 ? "No different files to migrate." : "Merge complete. Review the migrated settings before operating the machine.";
                    UpdateButtons();
                    MessageBox.Show(this, outcome + "\r\n\r\n" + backupDetails +
                        (result.Warnings.Count == 0 ? "" : "\r\n\r\n" + result.Warnings.Count + " warning(s) are shown in the details panel."),
                        "Merge complete", MessageBoxButtons.OK, result.Warnings.Count == 0 ? MessageBoxIcon.Information : MessageBoxIcon.Warning);
                });
        }

        private void StartWork(Func<BackgroundWorker, object> work, string message, bool isCopying, Action<object> completed)
        {
            if (busy) return;
            busy = true;
            copying = isCopying;
            progress.Visible = true;
            progress.Style = ProgressBarStyle.Marquee;
            status.Text = message;
            UpdateButtons();
            BackgroundWorker worker = new BackgroundWorker { WorkerReportsProgress = true };
            worker.DoWork += delegate(object sender, DoWorkEventArgs e) { e.Result = work(worker); };
            worker.ProgressChanged += delegate(object sender, ProgressChangedEventArgs e)
            {
                if (!IsDisposed) status.Text = Convert.ToString(e.UserState);
            };
            worker.RunWorkerCompleted += delegate(object sender, RunWorkerCompletedEventArgs e)
            {
                worker.Dispose();
                if (IsDisposed) return;
                busy = false;
                copying = false;
                progress.Visible = false;
                if (e.Error != null)
                {
                    if (isCopying)
                    {
                        plan = null;
                        optionalFiles.Items.Clear();
                        summary.Text = "The merge did not finish. Review the details and backup before trying again.";
                        Environment.ExitCode = 1;
                    }
                    details.Text = e.Error.Message;
                    status.Text = isCopying ? "Merge failed. Review the details below." : "Unable to complete the preview or search. See details.";
                    UpdateButtons();
                    ShowError(isCopying ? "Merge failed" : "Unable to continue", e.Error.Message);
                    return;
                }
                UpdateButtons();
                completed(e.Result);
            };
            worker.RunWorkerAsync();
        }

        private void ShowError(string title, string message)
        {
            MessageBox.Show(this, message, title, MessageBoxButtons.OK, MessageBoxIcon.Error);
        }

        private void ShowDataFiles()
        {
            if (plan == null) return;
            using (Form dialog = new Form())
            {
                dialog.Text = "New or different Data files included in the merge";
                dialog.StartPosition = FormStartPosition.CenterParent;
                dialog.Size = new Size(850, 500);
                dialog.MinimumSize = new Size(550, 300);
                dialog.Font = Font;
                ListView files = new ListView { Dock = DockStyle.Fill, View = View.Details, FullRowSelect = true, ShowItemToolTips = true };
                files.Columns.Add("File relative to installation", 575);
                files.Columns.Add("Action", 235);
                foreach (MigrationFile file in plan.DataFiles)
                {
                    ListViewItem item = new ListViewItem(file.RelativePath);
                    item.SubItems.Add(file.Overwrite ? "Back up and replace" : "Copy new file");
                    item.ToolTipText = "From: " + file.SourcePath + "\r\nTo: " + file.DestinationPath;
                    files.Items.Add(item);
                }
                Button done = MakeButton("Close");
                done.Dock = DockStyle.Bottom;
                done.DialogResult = DialogResult.OK;
                dialog.Controls.Add(files);
                dialog.Controls.Add(done);
                dialog.AcceptButton = done;
                dialog.CancelButton = done;
                dialog.ShowDialog(this);
            }
        }
    }
}
