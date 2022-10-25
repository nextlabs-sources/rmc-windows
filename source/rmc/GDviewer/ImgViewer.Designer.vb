<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()>
Partial Class MainForm
    Inherits System.Windows.Forms.Form

    'Form overrides dispose to clean up the component list.
    <System.Diagnostics.DebuggerNonUserCode()>
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
    <System.Diagnostics.DebuggerStepThrough()>
    Private Sub InitializeComponent()
        Dim resources As System.ComponentModel.ComponentResourceManager = New System.ComponentModel.ComponentResourceManager(GetType(MainForm))
        Me.MainFrameLayout = New System.Windows.Forms.TableLayoutPanel()
        Me.PageInfoPanel = New System.Windows.Forms.TableLayoutPanel()
        Me.PageDownPic = New System.Windows.Forms.PictureBox()
        Me.PageUpPic = New System.Windows.Forms.PictureBox()
        Me.PageNumLabel = New System.Windows.Forms.Label()
        Me.CurPageNumTxtBox = New System.Windows.Forms.TextBox()
        Me.PageTxtLabel = New System.Windows.Forms.Label()
        Me.FavorOfflinePanel = New System.Windows.Forms.TableLayoutPanel()
        Me.FavorPic = New System.Windows.Forms.PictureBox()
        Me.OfflinePic = New System.Windows.Forms.PictureBox()
        Me.OfflineLable = New System.Windows.Forms.Label()
        Me.FileInfoPanel = New System.Windows.Forms.TableLayoutPanel()
        Me.FileNameLabel = New System.Windows.Forms.Label()
        Me.FilePathLabel = New System.Windows.Forms.Label()
        Me.RenderingPicture = New System.Windows.Forms.PictureBox()
        Me.RightButtonPanel = New System.Windows.Forms.TableLayoutPanel()
        Me.PropertyPic = New System.Windows.Forms.PictureBox()
        Me.SharePic = New System.Windows.Forms.PictureBox()
        Me.ProtectPic = New System.Windows.Forms.PictureBox()
        Me.ZoomOutPic = New System.Windows.Forms.PictureBox()
        Me.ZoomInPic = New System.Windows.Forms.PictureBox()
        Me.Splitter1 = New System.Windows.Forms.Splitter()
        Me.MainFrameLayout.SuspendLayout()
        Me.PageInfoPanel.SuspendLayout()
        CType(Me.PageDownPic, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.PageUpPic, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.FavorOfflinePanel.SuspendLayout()
        CType(Me.FavorPic, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.OfflinePic, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.FileInfoPanel.SuspendLayout()
        CType(Me.RenderingPicture, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.RightButtonPanel.SuspendLayout()
        CType(Me.PropertyPic, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.SharePic, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.ProtectPic, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.ZoomOutPic, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.ZoomInPic, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.SuspendLayout()
        '
        'MainFrameLayout
        '
        Me.MainFrameLayout.BackColor = System.Drawing.SystemColors.ActiveCaptionText
        Me.MainFrameLayout.ColumnCount = 6
        Me.MainFrameLayout.ColumnStyles.Add(New System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 15.0!))
        Me.MainFrameLayout.ColumnStyles.Add(New System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 100.0!))
        Me.MainFrameLayout.ColumnStyles.Add(New System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 180.0!))
        Me.MainFrameLayout.ColumnStyles.Add(New System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 330.0!))
        Me.MainFrameLayout.ColumnStyles.Add(New System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 195.0!))
        Me.MainFrameLayout.ColumnStyles.Add(New System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 15.0!))
        Me.MainFrameLayout.Controls.Add(Me.PageInfoPanel, 3, 0)
        Me.MainFrameLayout.Controls.Add(Me.FavorOfflinePanel, 2, 0)
        Me.MainFrameLayout.Controls.Add(Me.FileInfoPanel, 1, 0)
        Me.MainFrameLayout.Controls.Add(Me.RenderingPicture, 0, 1)
        Me.MainFrameLayout.Controls.Add(Me.RightButtonPanel, 4, 0)
        Me.MainFrameLayout.Dock = System.Windows.Forms.DockStyle.Fill
        Me.MainFrameLayout.Location = New System.Drawing.Point(0, 0)
        Me.MainFrameLayout.Name = "MainFrameLayout"
        Me.MainFrameLayout.RowCount = 2
        Me.MainFrameLayout.RowStyles.Add(New System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 70.0!))
        Me.MainFrameLayout.RowStyles.Add(New System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 100.0!))
        Me.MainFrameLayout.Size = New System.Drawing.Size(921, 510)
        Me.MainFrameLayout.TabIndex = 0
        '
        'PageInfoPanel
        '
        Me.PageInfoPanel.Anchor = CType((System.Windows.Forms.AnchorStyles.Bottom Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.PageInfoPanel.BackColor = System.Drawing.SystemColors.ActiveCaptionText
        Me.PageInfoPanel.ColumnCount = 10
        Me.PageInfoPanel.ColumnStyles.Add(New System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 100.0!))
        Me.PageInfoPanel.ColumnStyles.Add(New System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 40.0!))
        Me.PageInfoPanel.ColumnStyles.Add(New System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 45.0!))
        Me.PageInfoPanel.ColumnStyles.Add(New System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 5.0!))
        Me.PageInfoPanel.ColumnStyles.Add(New System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 40.0!))
        Me.PageInfoPanel.ColumnStyles.Add(New System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 5.0!))
        Me.PageInfoPanel.ColumnStyles.Add(New System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 40.0!))
        Me.PageInfoPanel.ColumnStyles.Add(New System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 20.0!))
        Me.PageInfoPanel.ColumnStyles.Add(New System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 40.0!))
        Me.PageInfoPanel.ColumnStyles.Add(New System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 40.0!))
        Me.PageInfoPanel.Controls.Add(Me.PageDownPic, 6, 0)
        Me.PageInfoPanel.Controls.Add(Me.PageUpPic, 4, 0)
        Me.PageInfoPanel.Controls.Add(Me.PageNumLabel, 2, 0)
        Me.PageInfoPanel.Controls.Add(Me.CurPageNumTxtBox, 1, 0)
        Me.PageInfoPanel.Controls.Add(Me.PageTxtLabel, 0, 0)
        Me.PageInfoPanel.Controls.Add(Me.ZoomOutPic, 8, 0)
        Me.PageInfoPanel.Controls.Add(Me.ZoomInPic, 9, 0)
        Me.PageInfoPanel.Controls.Add(Me.Splitter1, 7, 0)
        Me.PageInfoPanel.Location = New System.Drawing.Point(384, 27)
        Me.PageInfoPanel.Name = "PageInfoPanel"
        Me.PageInfoPanel.RowCount = 1
        Me.PageInfoPanel.RowStyles.Add(New System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 100.0!))
        Me.PageInfoPanel.Size = New System.Drawing.Size(324, 40)
        Me.PageInfoPanel.TabIndex = 8
        '
        'PageDownPic
        '
        Me.PageDownPic.ErrorImage = Nothing
        Me.PageDownPic.Image = Global.GDviewer.My.Resources.Resources.page_down
        Me.PageDownPic.Location = New System.Drawing.Point(187, 3)
        Me.PageDownPic.Name = "PageDownPic"
        Me.PageDownPic.Size = New System.Drawing.Size(34, 34)
        Me.PageDownPic.TabIndex = 0
        Me.PageDownPic.TabStop = False
        '
        'PageUpPic
        '
        Me.PageUpPic.Image = Global.GDviewer.My.Resources.Resources.page_up
        Me.PageUpPic.InitialImage = Global.GDviewer.My.Resources.Resources.page_up
        Me.PageUpPic.Location = New System.Drawing.Point(142, 3)
        Me.PageUpPic.Name = "PageUpPic"
        Me.PageUpPic.Size = New System.Drawing.Size(34, 34)
        Me.PageUpPic.TabIndex = 1
        Me.PageUpPic.TabStop = False
        '
        'PageNumLabel
        '
        Me.PageNumLabel.Anchor = System.Windows.Forms.AnchorStyles.Left
        Me.PageNumLabel.AutoSize = True
        Me.PageNumLabel.ForeColor = System.Drawing.SystemColors.ButtonFace
        Me.PageNumLabel.Location = New System.Drawing.Point(89, 13)
        Me.PageNumLabel.Margin = New System.Windows.Forms.Padding(0)
        Me.PageNumLabel.Name = "PageNumLabel"
        Me.PageNumLabel.Size = New System.Drawing.Size(36, 13)
        Me.PageNumLabel.TabIndex = 2
        Me.PageNumLabel.Text = "/1234"
        Me.PageNumLabel.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
        '
        'CurPageNumTxtBox
        '
        Me.CurPageNumTxtBox.Anchor = System.Windows.Forms.AnchorStyles.Left
        Me.CurPageNumTxtBox.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle
        Me.CurPageNumTxtBox.Location = New System.Drawing.Point(49, 10)
        Me.CurPageNumTxtBox.Margin = New System.Windows.Forms.Padding(0)
        Me.CurPageNumTxtBox.MaxLength = 5
        Me.CurPageNumTxtBox.Name = "CurPageNumTxtBox"
        Me.CurPageNumTxtBox.RightToLeft = System.Windows.Forms.RightToLeft.Yes
        Me.CurPageNumTxtBox.Size = New System.Drawing.Size(40, 20)
        Me.CurPageNumTxtBox.TabIndex = 3
        Me.CurPageNumTxtBox.WordWrap = False
        '
        'PageTxtLabel
        '
        Me.PageTxtLabel.Anchor = System.Windows.Forms.AnchorStyles.Right
        Me.PageTxtLabel.AutoSize = True
        Me.PageTxtLabel.ForeColor = System.Drawing.SystemColors.ButtonFace
        Me.PageTxtLabel.Location = New System.Drawing.Point(9, 13)
        Me.PageTxtLabel.Name = "PageTxtLabel"
        Me.PageTxtLabel.Size = New System.Drawing.Size(37, 13)
        Me.PageTxtLabel.TabIndex = 4
        Me.PageTxtLabel.Text = "Pages"
        Me.PageTxtLabel.TextAlign = System.Drawing.ContentAlignment.MiddleRight
        '
        'FavorOfflinePanel
        '
        Me.FavorOfflinePanel.ColumnCount = 3
        Me.FavorOfflinePanel.ColumnStyles.Add(New System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 30.0!))
        Me.FavorOfflinePanel.ColumnStyles.Add(New System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 30.0!))
        Me.FavorOfflinePanel.ColumnStyles.Add(New System.Windows.Forms.ColumnStyle())
        Me.FavorOfflinePanel.Controls.Add(Me.FavorPic, 0, 1)
        Me.FavorOfflinePanel.Controls.Add(Me.OfflinePic, 1, 1)
        Me.FavorOfflinePanel.Controls.Add(Me.OfflineLable, 2, 1)
        Me.FavorOfflinePanel.Dock = System.Windows.Forms.DockStyle.Fill
        Me.FavorOfflinePanel.Location = New System.Drawing.Point(204, 3)
        Me.FavorOfflinePanel.Name = "FavorOfflinePanel"
        Me.FavorOfflinePanel.RowCount = 3
        Me.FavorOfflinePanel.RowStyles.Add(New System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 31.11111!))
        Me.FavorOfflinePanel.RowStyles.Add(New System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 68.88889!))
        Me.FavorOfflinePanel.RowStyles.Add(New System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 12.0!))
        Me.FavorOfflinePanel.Size = New System.Drawing.Size(174, 64)
        Me.FavorOfflinePanel.TabIndex = 6
        '
        'FavorPic
        '
        Me.FavorPic.Dock = System.Windows.Forms.DockStyle.Fill
        Me.FavorPic.Image = Global.GDviewer.My.Resources.Resources.fav_gray
        Me.FavorPic.Location = New System.Drawing.Point(3, 19)
        Me.FavorPic.Name = "FavorPic"
        Me.FavorPic.Size = New System.Drawing.Size(24, 29)
        Me.FavorPic.TabIndex = 0
        Me.FavorPic.TabStop = False
        '
        'OfflinePic
        '
        Me.OfflinePic.Image = Global.GDviewer.My.Resources.Resources.offline_file
        Me.OfflinePic.Location = New System.Drawing.Point(33, 19)
        Me.OfflinePic.Name = "OfflinePic"
        Me.OfflinePic.Size = New System.Drawing.Size(24, 25)
        Me.OfflinePic.TabIndex = 1
        Me.OfflinePic.TabStop = False
        '
        'OfflineLable
        '
        Me.OfflineLable.AutoEllipsis = True
        Me.OfflineLable.AutoSize = True
        Me.OfflineLable.Dock = System.Windows.Forms.DockStyle.Left
        Me.OfflineLable.Font = New System.Drawing.Font("Microsoft Sans Serif", 9.75!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.OfflineLable.ForeColor = System.Drawing.SystemColors.ControlDark
        Me.OfflineLable.Location = New System.Drawing.Point(63, 19)
        Me.OfflineLable.Margin = New System.Windows.Forms.Padding(3)
        Me.OfflineLable.Name = "OfflineLable"
        Me.OfflineLable.Size = New System.Drawing.Size(105, 29)
        Me.OfflineLable.TabIndex = 2
        Me.OfflineLable.Text = "Available Offline"
        Me.OfflineLable.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
        Me.OfflineLable.UseMnemonic = False
        '
        'FileInfoPanel
        '
        Me.FileInfoPanel.ColumnCount = 1
        Me.FileInfoPanel.ColumnStyles.Add(New System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 50.0!))
        Me.FileInfoPanel.Controls.Add(Me.FileNameLabel, 0, 1)
        Me.FileInfoPanel.Controls.Add(Me.FilePathLabel, 0, 0)
        Me.FileInfoPanel.Dock = System.Windows.Forms.DockStyle.Fill
        Me.FileInfoPanel.Location = New System.Drawing.Point(18, 3)
        Me.FileInfoPanel.Name = "FileInfoPanel"
        Me.FileInfoPanel.RowCount = 2
        Me.FileInfoPanel.RowStyles.Add(New System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 50.0!))
        Me.FileInfoPanel.RowStyles.Add(New System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 50.0!))
        Me.FileInfoPanel.Size = New System.Drawing.Size(180, 64)
        Me.FileInfoPanel.TabIndex = 7
        '
        'FileNameLabel
        '
        Me.FileNameLabel.AutoEllipsis = True
        Me.FileNameLabel.BackColor = System.Drawing.SystemColors.ActiveCaptionText
        Me.FileNameLabel.Dock = System.Windows.Forms.DockStyle.Bottom
        Me.FileNameLabel.Font = New System.Drawing.Font("Microsoft Sans Serif", 14.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.FileNameLabel.ForeColor = System.Drawing.SystemColors.Control
        Me.FileNameLabel.Location = New System.Drawing.Point(3, 40)
        Me.FileNameLabel.Name = "FileNameLabel"
        Me.FileNameLabel.Size = New System.Drawing.Size(174, 24)
        Me.FileNameLabel.TabIndex = 0
        Me.FileNameLabel.Text = "Essential with Axure edition.pdf"
        '
        'FilePathLabel
        '
        Me.FilePathLabel.AutoEllipsis = True
        Me.FilePathLabel.BackColor = System.Drawing.SystemColors.ActiveCaptionText
        Me.FilePathLabel.Dock = System.Windows.Forms.DockStyle.Bottom
        Me.FilePathLabel.Font = New System.Drawing.Font("Microsoft Sans Serif", 9.75!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.FilePathLabel.ForeColor = System.Drawing.SystemColors.GrayText
        Me.FilePathLabel.Location = New System.Drawing.Point(3, 14)
        Me.FilePathLabel.Name = "FilePathLabel"
        Me.FilePathLabel.Size = New System.Drawing.Size(174, 18)
        Me.FilePathLabel.TabIndex = 0
        Me.FilePathLabel.Text = "Office Drive\FY 2017\Finance\Asia Pacific\Manager\testing the long string effect " &
    "when resize the main window"
        Me.FilePathLabel.TextAlign = System.Drawing.ContentAlignment.BottomLeft
        Me.FilePathLabel.UseMnemonic = False
        '
        'RenderingPicture
        '
        Me.RenderingPicture.BackColor = System.Drawing.SystemColors.ButtonHighlight
        Me.MainFrameLayout.SetColumnSpan(Me.RenderingPicture, 6)
        Me.RenderingPicture.Dock = System.Windows.Forms.DockStyle.Fill
        Me.RenderingPicture.Image = Global.GDviewer.My.Resources.Resources.Loading_icon
        Me.RenderingPicture.InitialImage = Global.GDviewer.My.Resources.Resources.Loading_icon
        Me.RenderingPicture.Location = New System.Drawing.Point(0, 70)
        Me.RenderingPicture.Margin = New System.Windows.Forms.Padding(0)
        Me.RenderingPicture.Name = "RenderingPicture"
        Me.RenderingPicture.Padding = New System.Windows.Forms.Padding(0, 5, 0, 5)
        Me.RenderingPicture.Size = New System.Drawing.Size(921, 440)
        Me.RenderingPicture.SizeMode = System.Windows.Forms.PictureBoxSizeMode.CenterImage
        Me.RenderingPicture.TabIndex = 0
        Me.RenderingPicture.TabStop = False
        '
        'RightButtonPanel
        '
        Me.RightButtonPanel.ColumnCount = 3
        Me.RightButtonPanel.ColumnStyles.Add(New System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 65.0!))
        Me.RightButtonPanel.ColumnStyles.Add(New System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 65.0!))
        Me.RightButtonPanel.ColumnStyles.Add(New System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 65.0!))
        Me.RightButtonPanel.Controls.Add(Me.PropertyPic, 2, 0)
        Me.RightButtonPanel.Controls.Add(Me.SharePic, 0, 0)
        Me.RightButtonPanel.Controls.Add(Me.ProtectPic, 1, 0)
        Me.RightButtonPanel.Dock = System.Windows.Forms.DockStyle.Right
        Me.RightButtonPanel.Location = New System.Drawing.Point(711, 0)
        Me.RightButtonPanel.Margin = New System.Windows.Forms.Padding(0)
        Me.RightButtonPanel.Name = "RightButtonPanel"
        Me.RightButtonPanel.RowCount = 1
        Me.RightButtonPanel.RowStyles.Add(New System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 100.0!))
        Me.RightButtonPanel.Size = New System.Drawing.Size(195, 70)
        Me.RightButtonPanel.TabIndex = 4
        '
        'PropertyPic
        '
        Me.PropertyPic.ErrorImage = Nothing
        Me.PropertyPic.Image = Global.GDviewer.My.Resources.Resources.icon_func_properties_white
        Me.PropertyPic.InitialImage = Global.GDviewer.My.Resources.Resources.icon_func_properties_white
        Me.PropertyPic.Location = New System.Drawing.Point(132, 3)
        Me.PropertyPic.Margin = New System.Windows.Forms.Padding(2, 3, 2, 3)
        Me.PropertyPic.Name = "PropertyPic"
        Me.PropertyPic.Padding = New System.Windows.Forms.Padding(3, 1, 3, 1)
        Me.PropertyPic.Size = New System.Drawing.Size(61, 58)
        Me.PropertyPic.TabIndex = 2
        Me.PropertyPic.TabStop = False
        '
        'SharePic
        '
        Me.SharePic.ErrorImage = Nothing
        Me.SharePic.Image = Global.GDviewer.My.Resources.Resources.icon_func_share_white
        Me.SharePic.InitialImage = Global.GDviewer.My.Resources.Resources.icon_func_properties_white
        Me.SharePic.Location = New System.Drawing.Point(2, 3)
        Me.SharePic.Margin = New System.Windows.Forms.Padding(2, 3, 2, 3)
        Me.SharePic.Name = "SharePic"
        Me.SharePic.Size = New System.Drawing.Size(61, 58)
        Me.SharePic.TabIndex = 1
        Me.SharePic.TabStop = False
        '
        'ProtectPic
        '
        Me.ProtectPic.ErrorImage = Nothing
        Me.ProtectPic.Image = Global.GDviewer.My.Resources.Resources.icon_func_protect_white
        Me.ProtectPic.InitialImage = Global.GDviewer.My.Resources.Resources.icon_func_properties_white
        Me.ProtectPic.Location = New System.Drawing.Point(67, 3)
        Me.ProtectPic.Margin = New System.Windows.Forms.Padding(2, 3, 2, 3)
        Me.ProtectPic.Name = "ProtectPic"
        Me.ProtectPic.Padding = New System.Windows.Forms.Padding(3, 1, 3, 1)
        Me.ProtectPic.Size = New System.Drawing.Size(61, 58)
        Me.ProtectPic.TabIndex = 3
        Me.ProtectPic.TabStop = False
        '
        'ZoomOutPic
        '
        Me.ZoomOutPic.ErrorImage = Nothing
        Me.ZoomOutPic.Image = Global.GDviewer.My.Resources.Resources.Zoom_out
        Me.ZoomOutPic.Location = New System.Drawing.Point(247, 3)
        Me.ZoomOutPic.Name = "ZoomOutPic"
        Me.ZoomOutPic.Size = New System.Drawing.Size(34, 34)
        Me.ZoomOutPic.TabIndex = 5
        Me.ZoomOutPic.TabStop = False
        '
        'ZoomInPic
        '
        Me.ZoomInPic.Image = Global.GDviewer.My.Resources.Resources.Zoom_in
        Me.ZoomInPic.Location = New System.Drawing.Point(287, 3)
        Me.ZoomInPic.Name = "ZoomInPic"
        Me.ZoomInPic.Size = New System.Drawing.Size(34, 34)
        Me.ZoomInPic.TabIndex = 6
        Me.ZoomInPic.TabStop = False
        '
        'Splitter1
        '
        Me.Splitter1.BackColor = System.Drawing.SystemColors.ButtonHighlight
        Me.Splitter1.Dock = System.Windows.Forms.DockStyle.Right
        Me.Splitter1.Location = New System.Drawing.Point(231, 10)
        Me.Splitter1.Margin = New System.Windows.Forms.Padding(3, 10, 10, 10)
        Me.Splitter1.Name = "Splitter1"
        Me.Splitter1.Size = New System.Drawing.Size(3, 20)
        Me.Splitter1.TabIndex = 7
        Me.Splitter1.TabStop = False
        '
        'MainForm
        '
        Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0!, 13.0!)
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
        Me.BackColor = System.Drawing.SystemColors.ActiveCaptionText
        Me.ClientSize = New System.Drawing.Size(921, 510)
        Me.Controls.Add(Me.MainFrameLayout)
        Me.Icon = CType(resources.GetObject("$this.Icon"), System.Drawing.Icon)
        Me.MinimumSize = New System.Drawing.Size(840, 450)
        Me.Name = "MainForm"
        Me.Text = "NextLabs - SkyDRM"
        Me.MainFrameLayout.ResumeLayout(False)
        Me.PageInfoPanel.ResumeLayout(False)
        Me.PageInfoPanel.PerformLayout()
        CType(Me.PageDownPic, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.PageUpPic, System.ComponentModel.ISupportInitialize).EndInit()
        Me.FavorOfflinePanel.ResumeLayout(False)
        Me.FavorOfflinePanel.PerformLayout()
        CType(Me.FavorPic, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.OfflinePic, System.ComponentModel.ISupportInitialize).EndInit()
        Me.FileInfoPanel.ResumeLayout(False)
        CType(Me.RenderingPicture, System.ComponentModel.ISupportInitialize).EndInit()
        Me.RightButtonPanel.ResumeLayout(False)
        CType(Me.PropertyPic, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.SharePic, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.ProtectPic, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.ZoomOutPic, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.ZoomInPic, System.ComponentModel.ISupportInitialize).EndInit()
        Me.ResumeLayout(False)

    End Sub

    Friend WithEvents MainFrameLayout As TableLayoutPanel
    Friend WithEvents ProtectPic As PictureBox
    Friend WithEvents RenderingPicture As PictureBox
    Friend WithEvents PropertyPic As PictureBox
    Friend WithEvents SharePic As PictureBox
    Friend WithEvents RightButtonPanel As TableLayoutPanel
    Friend WithEvents FilePathLabel As Label
    Friend WithEvents FileNameLabel As Label
    Friend WithEvents FavorOfflinePanel As TableLayoutPanel
    Friend WithEvents FavorPic As PictureBox
    Friend WithEvents OfflinePic As PictureBox
    Friend WithEvents OfflineLable As Label
    Friend WithEvents FileInfoPanel As TableLayoutPanel
    Friend WithEvents PageInfoPanel As TableLayoutPanel
    Friend WithEvents PageDownPic As PictureBox
    Friend WithEvents PageUpPic As PictureBox
    Friend WithEvents PageNumLabel As Label
    Friend WithEvents CurPageNumTxtBox As TextBox
    Friend WithEvents PageTxtLabel As Label
    Friend WithEvents ZoomOutPic As PictureBox
    Friend WithEvents ZoomInPic As PictureBox
    Friend WithEvents Splitter1 As Splitter
End Class
