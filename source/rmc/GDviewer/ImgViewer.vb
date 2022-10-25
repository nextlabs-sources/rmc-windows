Imports System
Imports System.ComponentModel
Imports System.IO

Public Class MainForm
    Dim m_TotalPageNum As Integer
    Dim m_CurrentPageNum As Integer
    Dim m_DocRender As GroupDocs.Viewer.Rendering.ViewGenerator
    Dim m_RenderThread As System.Threading.Thread
    Dim m_NxlHelper As Nextlabs.GDViewer.NxlFileHelper = New Nextlabs.GDViewer.NxlFileHelper

    Private Const strViewerType As String = "X_NXRM_VIEWER_TYPE"
    Private Const strViewerParams As String = "X_NXRM_VIEWER_PARAMS"
    Private Sub EnableControls(enabled As Boolean)
        SharePic.Enabled = enabled
        PropertyPic.Enabled = enabled
        ProtectPic.Enabled = enabled
        FavorPic.Enabled = enabled
        OfflinePic.Enabled = enabled
        CurPageNumTxtBox.Enabled = enabled
        PageUpPic.Enabled = enabled
        PageDownPic.Enabled = enabled
    End Sub
    Private Sub ShowCurPage()
        PageUpPic.Enabled = True
        PageDownPic.Enabled = True
        If (m_CurrentPageNum = 1) Then
            PageUpPic.Enabled = False
        End If
        If (m_CurrentPageNum = m_TotalPageNum) Then
            PageDownPic.Enabled = False
        End If
        CurPageNumTxtBox.Text = m_CurrentPageNum.ToString()
        RenderingPicture.Image = Image.FromStream(m_DocRender.GetImage(m_CurrentPageNum - 1))
        RenderingPicture.SizeMode = PictureBoxSizeMode.Zoom
        RenderingPicture.BackColor = SystemColors.AppWorkspace
    End Sub
    Delegate Sub StringArgReturningVoidDelegate([curPage] As String, [totalPage] As String)

    Private Sub SetPageText(ByVal [curPage] As String, ByVal [totalPage] As String)

        ' InvokeRequired required compares the thread ID of the
        ' calling thread to the thread ID of the creating thread.
        ' If these threads are different, it returns true.
        If Me.PageNumLabel.InvokeRequired And Me.PageNumLabel.InvokeRequired Then
            Dim d As New StringArgReturningVoidDelegate(AddressOf SetPageText)
            Me.Invoke(d, New Object() {[curPage], [totalPage]})
        Else
            If (m_TotalPageNum = 0) Then
                MessageBox.Show("Not Supported File Format!", "Error", MessageBoxButtons.OK, MessageBoxIcon.Stop)
                RenderingPicture.Load(MessageBoxIcon.Stop)
                RenderingPicture.SizeMode = PictureBoxSizeMode.CenterImage
                RenderingPicture.BackColor = SystemColors.ButtonHighlight
                Me.PageNumLabel.Text = ""
                Me.CurPageNumTxtBox.Text = ""
                EnableControls(False)
            Else
                Me.PageNumLabel.Text = totalPage
                Me.CurPageNumTxtBox.Text = curPage
                EnableControls(True)
                ShowCurPage()
            End If
        End If
    End Sub
    Private Sub InitializeRenderEngine()
        'm_DocRender.RenderDocument()
        m_DocRender.RenderDocumentWithWatermark("NextLabs Rmc", Color.Purple)
        If m_DocRender.PageNum() = 0 Then
            Return
        End If

        m_TotalPageNum = m_DocRender.PageNum
        m_CurrentPageNum = 1
        Me.SetPageText(m_CurrentPageNum.ToString(), "/" + m_TotalPageNum.ToString())
    End Sub
    Private Sub SharePic_Click(sender As Object, e As EventArgs) Handles SharePic.Click
        If (m_RenderThread.IsAlive()) Then
            Return
        End If

        m_NxlHelper.ShareFile(Me.Handle)
    End Sub


    Private Sub PropertyPic_Click(sender As Object, e As EventArgs) Handles PropertyPic.Click
        If (m_RenderThread.IsAlive()) Then
            Return
        End If
        m_NxlHelper.ShowFileInfo(Me.Handle)
    End Sub

    Private Sub ProtectPic_Click(sender As Object, e As EventArgs) Handles ProtectPic.Click
        If (m_RenderThread.IsAlive()) Then
            Return
        End If
        m_NxlHelper.ProtectFile(Me.Handle)
    End Sub
    Private Sub MainForm_Load(sender As Object, e As EventArgs) Handles Me.Load
        Dim viewerType As String
        Dim viewerParam As String
        viewerType = Environment.GetEnvironmentVariable(strViewerType)
        viewerParam = Environment.GetEnvironmentVariable(strViewerParams)

        'viewerType = "Local"
        'viewerParam = "{""viewerType"":1," &
        '"""tempFilePath"":""c:\\storage\\AB000000000000000""," &
        '"""filePath"":""/MyDrive/Samples/xxx.docx.nxl""," &
        '"""fileId"":""/82822-928292-33/samples/xxx.docx.nxl""," &
        '"""fileUid"":""10638392392829292828282 <-- 32 characters""," &
        '"""originalFileName"":""test.docx""," &
        '"""lastModifiedTime"":18379447484," &
        '"""fileSize"":10000," &
        '"""offline"":false," &
        '"""favorite"":false," &
        '"""local"":true," &
        '"""tempFileEncrypted"":false }"

        If String.IsNullOrEmpty(viewerType) Or String.IsNullOrEmpty(viewerParam) Then
            MessageBox.Show("Incompatible error detected!", "Error", MessageBoxButtons.OK, MessageBoxIcon.Stop)
            Application.Exit()
            Return
        End If
        If Not (m_NxlHelper.ParseVarible(viewerType, viewerParam)) Then
            MessageBox.Show(m_NxlHelper.strErr, "Error", MessageBoxButtons.OK, MessageBoxIcon.Stop)
            Application.Exit()
            Return
        End If

        Dim fileName = m_NxlHelper.strFilePath
        If (m_NxlHelper.bLocal) Then
            FilePathLabel.Text = Path.GetFullPath(m_NxlHelper.strFileShowPath)
            FileNameLabel.Text = Path.GetFileName(m_NxlHelper.strFileShowPath)

        End If
        FilePathLabel.Text = m_NxlHelper.strFileShowPath
        FileNameLabel.Text = m_NxlHelper.strFileShowName
        OfflineLable.Text = ""
        If (m_NxlHelper.bFavorite) Then
            FavorPic.Image = Global.GDviewer.My.Resources.Resources.fav_marked
        Else
            FavorPic.Image = Global.GDviewer.My.Resources.Resources.fav_gray
        End If

        If (m_NxlHelper.bOffline) Then
            OfflinePic.Image = Global.GDviewer.My.Resources.Resources.offline_file
        Else
            OfflinePic.Image = Global.GDviewer.My.Resources.Resources.offline_file_gray
        End If
        Try
            Dim fInfo As System.IO.FileInfo
            fInfo = My.Computer.FileSystem.GetFileInfo(fileName)
            fileName = fileName + Path.GetExtension(m_NxlHelper.strFileShowName)
            My.Computer.FileSystem.MoveFile(m_NxlHelper.strFilePath, fileName)
        Catch ex As Exception

        End Try

        m_DocRender = New GroupDocs.Viewer.Rendering.ViewGenerator()
        Dim retMsg As String = m_DocRender.SetDocument(fileName)
        If Not [String].IsNullOrEmpty(retMsg) Then
            MessageBox.Show(retMsg)
            Application.Exit()
        End If
        EnableControls(False)
        PageNumLabel.Text = "/"
        m_RenderThread = New System.Threading.Thread(AddressOf InitializeRenderEngine)
        m_RenderThread.Start()
    End Sub

    Private Sub MainForm_Closing(sender As Object, e As CancelEventArgs) Handles Me.Closing
        If (m_RenderThread.IsAlive()) Then
            m_RenderThread.Abort()
        End If
    End Sub

    Private Sub MainForm_Disposed(sender As Object, e As EventArgs) Handles Me.Disposed
        Try
            m_DocRender.DeleteRenderedImageFiles()
        Catch ex As Exception

        End Try
    End Sub

    Private Sub CurPageNumTxtBox_TextChanged(sender As Object, e As EventArgs) Handles CurPageNumTxtBox.TextChanged
        Dim inputnum As Integer
        inputnum = Integer.Parse(CurPageNumTxtBox.Text)
        If (inputnum = m_CurrentPageNum) Then
            Return
        End If

        If (inputnum > m_TotalPageNum) Then
            inputnum = m_TotalPageNum
            CurPageNumTxtBox.Text = inputnum.ToString()
        ElseIf (inputnum <= 0) Then
            inputnum = 1
            CurPageNumTxtBox.Text = inputnum.ToString()
        Else
        End If
        m_CurrentPageNum = inputnum
        ShowCurPage()
    End Sub

    Private Sub CurPageNumTxtBox_KeyDown(sender As Object, e As KeyEventArgs) Handles CurPageNumTxtBox.KeyDown
        If Asc(e.KeyValue) <> 13 AndAlso Asc(e.KeyValue) <> 8 AndAlso Not IsNumeric(e.KeyValue) Then
            'ignore other characters
            e.Handled = True
        End If
    End Sub

    Private Sub PageUpPic_Click(sender As Object, e As EventArgs) Handles PageUpPic.Click
        If (m_RenderThread.IsAlive()) Then
            Return
        End If
        m_CurrentPageNum = m_CurrentPageNum - 1
        ShowCurPage()
    End Sub

    Private Sub PageDownPic_Click(sender As Object, e As EventArgs) Handles PageDownPic.Click
        If (m_RenderThread.IsAlive()) Then
            Return
        End If
        m_CurrentPageNum = m_CurrentPageNum + 1
        ShowCurPage()
    End Sub
End Class
