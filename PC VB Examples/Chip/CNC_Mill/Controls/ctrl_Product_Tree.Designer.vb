<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class ctrl_Product_Tree
    Inherits System.Windows.Forms.UserControl

    'UserControl overrides dispose to clean up the component list.
    <System.Diagnostics.DebuggerNonUserCode()> _
    Protected Overrides Sub Dispose(ByVal disposing As Boolean)
        Try
            If disposing AndAlso components IsNot Nothing Then
                components.Dispose()
            End If
        Finally
            MyBase.Dispose(disposing)
        End Try
    End Sub

    'Required by the Windows Form Designer
    Private components As System.ComponentModel.IContainer

    'NOTE: The following procedure is required by the Windows Form Designer
    'It can be modified using the Windows Form Designer.  
    'Do not modify it using the code editor.
    <System.Diagnostics.DebuggerStepThrough()> _
    Private Sub InitializeComponent()
        Dim resources As System.ComponentModel.ComponentResourceManager = New System.ComponentModel.ComponentResourceManager(GetType(ctrl_Product_Tree))
        Me.tree_Products = New System.Windows.Forms.TreeView()
        Me.ts_Tree = New System.Windows.Forms.ToolStrip()
        Me.ts_Tree_Save = New System.Windows.Forms.ToolStripButton()
        Me.ts_Tree_Edit_Node = New System.Windows.Forms.ToolStripButton()
        Me.ts_Tree_Insert_Header = New System.Windows.Forms.ToolStripButton()
        Me.ts_Tree_Insert_Node = New System.Windows.Forms.ToolStripButton()
        Me.ts_Tree_Delete_Node = New System.Windows.Forms.ToolStripButton()
        Me.ts_Tree_Copy = New System.Windows.Forms.ToolStripButton()
        Me.ts_Tree_Up = New System.Windows.Forms.ToolStripButton()
        Me.ts_Tree_Down = New System.Windows.Forms.ToolStripButton()
        Me.ToolStripButton1 = New System.Windows.Forms.ToolStripButton()
        Me.ts_Tree.SuspendLayout()
        Me.SuspendLayout()
        '
        'tree_Products
        '
        Me.tree_Products.Dock = System.Windows.Forms.DockStyle.Fill
        Me.tree_Products.Location = New System.Drawing.Point(0, 38)
        Me.tree_Products.Name = "tree_Products"
        Me.tree_Products.Size = New System.Drawing.Size(678, 410)
        Me.tree_Products.TabIndex = 0
        '
        'ts_Tree
        '
        Me.ts_Tree.Items.AddRange(New System.Windows.Forms.ToolStripItem() {Me.ts_Tree_Save, Me.ts_Tree_Edit_Node, Me.ts_Tree_Insert_Header, Me.ts_Tree_Insert_Node, Me.ts_Tree_Delete_Node, Me.ts_Tree_Copy, Me.ts_Tree_Up, Me.ts_Tree_Down, Me.ToolStripButton1})
        Me.ts_Tree.Location = New System.Drawing.Point(0, 0)
        Me.ts_Tree.Name = "ts_Tree"
        Me.ts_Tree.Size = New System.Drawing.Size(678, 38)
        Me.ts_Tree.TabIndex = 2
        Me.ts_Tree.Text = "ToolStrip1"
        '
        'ts_Tree_Save
        '
        Me.ts_Tree_Save.Image = CType(resources.GetObject("ts_Tree_Save.Image"), System.Drawing.Image)
        Me.ts_Tree_Save.ImageTransparentColor = System.Drawing.Color.Magenta
        Me.ts_Tree_Save.Name = "ts_Tree_Save"
        Me.ts_Tree_Save.Size = New System.Drawing.Size(35, 35)
        Me.ts_Tree_Save.Text = "Save"
        Me.ts_Tree_Save.TextImageRelation = System.Windows.Forms.TextImageRelation.ImageAboveText
        '
        'ts_Tree_Edit_Node
        '
        Me.ts_Tree_Edit_Node.Image = CType(resources.GetObject("ts_Tree_Edit_Node.Image"), System.Drawing.Image)
        Me.ts_Tree_Edit_Node.ImageTransparentColor = System.Drawing.Color.Magenta
        Me.ts_Tree_Edit_Node.Name = "ts_Tree_Edit_Node"
        Me.ts_Tree_Edit_Node.Size = New System.Drawing.Size(31, 35)
        Me.ts_Tree_Edit_Node.Text = "Edit"
        Me.ts_Tree_Edit_Node.TextImageRelation = System.Windows.Forms.TextImageRelation.ImageAboveText
        '
        'ts_Tree_Insert_Header
        '
        Me.ts_Tree_Insert_Header.Image = CType(resources.GetObject("ts_Tree_Insert_Header.Image"), System.Drawing.Image)
        Me.ts_Tree_Insert_Header.ImageTransparentColor = System.Drawing.Color.Magenta
        Me.ts_Tree_Insert_Header.Name = "ts_Tree_Insert_Header"
        Me.ts_Tree_Insert_Header.Size = New System.Drawing.Size(81, 35)
        Me.ts_Tree_Insert_Header.Text = "Insert Header"
        Me.ts_Tree_Insert_Header.TextImageRelation = System.Windows.Forms.TextImageRelation.ImageAboveText
        '
        'ts_Tree_Insert_Node
        '
        Me.ts_Tree_Insert_Node.Image = CType(resources.GetObject("ts_Tree_Insert_Node.Image"), System.Drawing.Image)
        Me.ts_Tree_Insert_Node.ImageTransparentColor = System.Drawing.Color.Magenta
        Me.ts_Tree_Insert_Node.Name = "ts_Tree_Insert_Node"
        Me.ts_Tree_Insert_Node.Size = New System.Drawing.Size(72, 35)
        Me.ts_Tree_Insert_Node.Text = "Insert Node"
        Me.ts_Tree_Insert_Node.TextImageRelation = System.Windows.Forms.TextImageRelation.ImageAboveText
        '
        'ts_Tree_Delete_Node
        '
        Me.ts_Tree_Delete_Node.Image = CType(resources.GetObject("ts_Tree_Delete_Node.Image"), System.Drawing.Image)
        Me.ts_Tree_Delete_Node.ImageTransparentColor = System.Drawing.Color.Magenta
        Me.ts_Tree_Delete_Node.Name = "ts_Tree_Delete_Node"
        Me.ts_Tree_Delete_Node.Size = New System.Drawing.Size(44, 35)
        Me.ts_Tree_Delete_Node.Text = "Delete"
        Me.ts_Tree_Delete_Node.TextImageRelation = System.Windows.Forms.TextImageRelation.ImageAboveText
        '
        'ts_Tree_Copy
        '
        Me.ts_Tree_Copy.Image = CType(resources.GetObject("ts_Tree_Copy.Image"), System.Drawing.Image)
        Me.ts_Tree_Copy.ImageTransparentColor = System.Drawing.Color.Magenta
        Me.ts_Tree_Copy.Name = "ts_Tree_Copy"
        Me.ts_Tree_Copy.Size = New System.Drawing.Size(39, 35)
        Me.ts_Tree_Copy.Text = "Copy"
        Me.ts_Tree_Copy.TextImageRelation = System.Windows.Forms.TextImageRelation.ImageAboveText
        '
        'ts_Tree_Up
        '
        Me.ts_Tree_Up.Image = CType(resources.GetObject("ts_Tree_Up.Image"), System.Drawing.Image)
        Me.ts_Tree_Up.ImageTransparentColor = System.Drawing.Color.Magenta
        Me.ts_Tree_Up.Name = "ts_Tree_Up"
        Me.ts_Tree_Up.Size = New System.Drawing.Size(59, 35)
        Me.ts_Tree_Up.Text = "Move Up"
        Me.ts_Tree_Up.TextImageRelation = System.Windows.Forms.TextImageRelation.ImageAboveText
        '
        'ts_Tree_Down
        '
        Me.ts_Tree_Down.Image = CType(resources.GetObject("ts_Tree_Down.Image"), System.Drawing.Image)
        Me.ts_Tree_Down.ImageTransparentColor = System.Drawing.Color.Magenta
        Me.ts_Tree_Down.Name = "ts_Tree_Down"
        Me.ts_Tree_Down.Size = New System.Drawing.Size(75, 35)
        Me.ts_Tree_Down.Text = "Move Down"
        Me.ts_Tree_Down.TextImageRelation = System.Windows.Forms.TextImageRelation.ImageAboveText
        '
        'ToolStripButton1
        '
        Me.ToolStripButton1.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image
        Me.ToolStripButton1.Image = CType(resources.GetObject("ToolStripButton1.Image"), System.Drawing.Image)
        Me.ToolStripButton1.ImageTransparentColor = System.Drawing.Color.Magenta
        Me.ToolStripButton1.Name = "ToolStripButton1"
        Me.ToolStripButton1.Size = New System.Drawing.Size(23, 35)
        Me.ToolStripButton1.Text = "ToolStripButton1"
        '
        'ctrl_Product_Tree
        '
        Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0!, 13.0!)
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
        Me.Controls.Add(Me.tree_Products)
        Me.Controls.Add(Me.ts_Tree)
        Me.Name = "ctrl_Product_Tree"
        Me.Size = New System.Drawing.Size(678, 448)
        Me.ts_Tree.ResumeLayout(False)
        Me.ts_Tree.PerformLayout()
        Me.ResumeLayout(False)
        Me.PerformLayout()

    End Sub

    Friend WithEvents tree_Products As TreeView
    Friend WithEvents ts_Tree As ToolStrip
    Friend WithEvents ts_Tree_Save As ToolStripButton
    Friend WithEvents ts_Tree_Edit_Node As ToolStripButton
    Friend WithEvents ts_Tree_Insert_Header As ToolStripButton
    Friend WithEvents ts_Tree_Insert_Node As ToolStripButton
    Friend WithEvents ts_Tree_Delete_Node As ToolStripButton
    Friend WithEvents ts_Tree_Copy As ToolStripButton
    Friend WithEvents ts_Tree_Up As ToolStripButton
    Friend WithEvents ts_Tree_Down As ToolStripButton
    Friend WithEvents ToolStripButton1 As ToolStripButton
End Class
