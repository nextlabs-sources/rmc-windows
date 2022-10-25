
Imports System.Collections.Generic
Imports System.Linq
Imports System.Text
Imports GroupDocs.Viewer.Config
Imports GroupDocs.Viewer.Handler
Imports GroupDocs.Viewer.Converter.Options
Imports GroupDocs.Viewer.Domain.Html
Imports GroupDocs.Viewer.Domain.Image
Imports GroupDocs.Viewer.Domain.Options
Imports System.Drawing
Imports GroupDocs.Viewer.Domain
Imports GroupDocs.Viewer.Domain.Containers
Imports System.IO
Imports GroupDocs.Viewer.Domain.Transformation
Imports GroupDocs.Viewer.Handler.Input
Imports System.Globalization


Namespace GroupDocs.Viewer.Rendering
    Public Enum DocumentType
        UnknownType = 0
        AdobePdf = 1
        OfficeWord = 2
        OfficeExcel = 3
        OfficePPT = 4
        OfficeViso = 5
        Text = 6
        Image = 7
    End Enum
    Public NotInheritable Class ViewGenerator
        Private m_Config As ViewerConfig
        Private m_FileName As String
        Private m_FileFullPath As String
        Private m_DocType As DocumentType
        Private m_ImgHandler As ViewerImageHandler
        Private m_ImageFileNameArr() As String
        Private m_ErrorMsg As String
        Private m_DocPageNumber As Integer
        Private m_ImagesArray() As PageImage

        Public Sub New()
            m_config = Utilities.GetConfigurations()
            m_imgHandler = New ViewerImageHandler(m_config)
            m_FileName = ""
            m_FileFullPath = ""
            m_DocPageNumber = 0
            Utilities.ApplyLicense()
            m_DocType = DocumentType.UnknownType
        End Sub

        ''' <summary>
        ''' 
        ''' </summary>
        ''' <param name="DocumentName">the full file name for rendering</param>
        ''' <returns>NULL string if success. Otherwise return the error string</returns>
        Public Function SetDocument(DocumentName As [String]) As String
            'reset file information
            If (m_FileName.Length > 0) Then
                RemoveCacheFiles()
                m_FileName = ""
                m_DocPageNumber = 0
            End If
            If (Not GetDocumentInfo(DocumentName)) Then
                Return m_ErrorMsg
            End If
            m_FileFullPath = DocumentName
            Return ""
        End Function
        Public Function PageNum() As Integer
            Return m_DocPageNumber
        End Function

        Public Sub DeleteRenderedImageFiles()
            For Each filename As String In m_ImageFileNameArr
                If (Not [String].IsNullOrEmpty(filename)) Then
                    Try
                        My.Computer.FileSystem.DeleteFile(filename)
                    Catch exp As System.Exception
                        m_ErrorMsg = exp.Message
                    End Try
                End If
            Next
        End Sub

        ''' <summary>
        ''' 
        ''' </summary>
        ''' <param name="index">index start from 0 to PageNum-1 </param>
        ''' <returns></returns>
        Public Function GetImagePath(index As Integer) As String
            If ([String].IsNullOrEmpty(m_ImageFileNameArr(index))) Then
                For i As Integer = 0 To 5
                    'Save maximum 5 images at disk at one time
                    If (index + i >= m_DocPageNumber) Then
                        Exit For
                    End If
                    Dim retstr As String
                    retstr = Utilities.SaveAsImage((index + i + 1) & "_" & m_FileName, m_Config.StoragePath, m_ImagesArray(index + i).Stream)
                    If String.IsNullOrEmpty(retstr) Then
                        m_DocPageNumber = index + i + 1
                        Return Nothing
                    End If
                    m_ImageFileNameArr(index + i) = retstr
                Next
            End If
            Return m_ImageFileNameArr(index)
        End Function

        Public Function GetImage(index As Integer) As Stream
            If (index >= m_DocPageNumber) Then
                Return Nothing
            End If
            Return m_ImagesArray(index).Stream
        End Function

#Region "ImageRepresentation"
        ''' <summary>
        ''' Render simple document in image representation
        ''' </summary>
        ''' <param name="DocumentPassword">Optional</param>
        Public Sub RenderDocument(Optional DocumentPassword As [String] = Nothing)
            'ExStart:RenderAsImage
            If [String].IsNullOrEmpty(m_FileName) Then
                Return
            End If

            'Initialize ImageOptions Object
            Dim options As ImageOptions = Utilities.GetOptionByType(m_DocType, DocumentPassword)

            Dim images As New List(Of PageImage)
            'Get document pages in image form
            images = m_ImgHandler.GetPages(m_FileFullPath, options)
            If images.Count <> m_DocPageNumber Then
                m_DocPageNumber = images.Count
                ReDim m_ImageFileNameArr(m_DocPageNumber)
            End If

            m_ImagesArray = images.ToArray

            'My.Computer.FileSystem.DeleteFile(m_FileFullPath)
        End Sub
        ''' <summary>
        ''' Render document in image representation with watermark
        ''' </summary>
        ''' <param name="WatermarkText">watermark text</param>
        ''' <param name="WatermarkColor"> System.Drawing.Color</param>
        ''' <param name="position">Watermark Position is optional parameter. Default value is WatermarkPosition.Diagonal</param>
        ''' <param name="WatermarkWidth"> width of watermark as integer. it is optional Parameter default value is 100</param>
        ''' <param name="DocumentPassword">Password Parameter is optional</param>
        Public Sub RenderDocumentWithWatermark(WatermarkText As [String], WatermarkColor As Color, Optional position As WatermarkPosition = WatermarkPosition.Diagonal, Optional WatermarkWidth As Integer = 100, Optional DocumentPassword As [String] = Nothing)
            'ExStart:RenderAsImageWithWaterMark
            If [String].IsNullOrEmpty(m_FileName) Then
                Return
            End If

            'Initialize ImageOptions Object
            Dim options As ImageOptions = Utilities.GetOptionByType(m_DocType, DocumentPassword)

            ' Call AddWatermark and pass the reference of ImageOptions object as 1st parameter
            Utilities.PageTransformations.AddWatermark(options, WatermarkText, WatermarkColor, position, WatermarkWidth)

            'Get document pages in image form
            'Get document pages in image form
            If m_DocPageNumber > 5 Then
                options.PageNumbersToRender = Enumerable.Range(1, 5).ToList()
            End If

            Dim images As New List(Of PageImage)
            'Get document pages in image form
            images = m_ImgHandler.GetPages(m_FileFullPath, options)
            If images.Count <> m_DocPageNumber Then
                m_DocPageNumber = images.Count
                ReDim m_ImageFileNameArr(m_DocPageNumber)
            End If

            m_ImagesArray = images.ToArray

            'My.Computer.FileSystem.DeleteFile(m_FileFullPath)

            'Save each image at disk
            'For Each image As PageImage In images
            'Dim retstr As String
            'retstr = Utilities.SaveAsImage(image.PageNumber & "_" & m_FileName, m_Config.StoragePath, image.Stream)
            'If String.IsNullOrEmpty(retstr) Then
            'm_DocPageNumber = image.PageNumber - 1
            'Return
            'End If
            'm_ImageFileNameArr(image.PageNumber - 1) = retstr
            'If (image.PageNumber >= 5) Then
            'for performance, only save 5 pictures at beginning
            'Exit For
            'End If
            'Next
            'ExEnd:RenderAsImageWithWaterMark
        End Sub
#End Region

#Region "DocumentInformation"
        ''' <summary>
        ''' Get document information by name
        ''' </summary>
        ''' <param name="DocumentName">document name</param>
        Private Function GetDocumentInfo(DocumentName As [String]) As Boolean
            Dim bret As Boolean = False
            Try
                'ExStart:GetDocumentInfoByGuid
                ' Get document information
                ' Get supported document formats
                Dim documentFormatsContainer As DocumentFormatsContainer = m_imgHandler.GetSupportedDocumentFormats()
                Dim supportedDocumentFormats As Dictionary(Of String, String) = documentFormatsContainer.SupportedDocumentFormats

                Dim fileExtension As String = Path.GetExtension(DocumentName).Remove(0, 1)

                For Each supportedDocumentFormat As KeyValuePair(Of String, String) In supportedDocumentFormats
                    If [String].Equals(supportedDocumentFormat.Key, fileExtension, StringComparison.OrdinalIgnoreCase) Then
                        Dim options As New DocumentInfoOptions()

                        If supportedDocumentFormat.Value.Contains("Excel") Then
                            m_DocType = DocumentType.OfficeExcel
                            options.CellsOptions.OnePagePerSheet = False
                            options.CellsOptions.ShowGridLines = True
                            options.CellsOptions.CountRowsPerPage = 50
                        ElseIf supportedDocumentFormat.Value.Contains("Word") Then
                            m_DocType = DocumentType.OfficeWord
                        ElseIf supportedDocumentFormat.Value.Contains("PowerPoint") Then
                            m_DocType = DocumentType.OfficePPT
                        ElseIf supportedDocumentFormat.Value.Contains("Portable Document Format") Then
                            m_DocType = DocumentType.AdobePdf
                        ElseIf supportedDocumentFormat.Value.Contains("Text") Then
                            m_DocType = DocumentType.Text
                        End If

                        m_FileName = Path.GetFileName(DocumentName)
                        bret = True
                        Exit For
                    End If
                    Console.WriteLine(String.Format("Extension: '{0}'; Document format: '{1}'", supportedDocumentFormat.Key, supportedDocumentFormat.Value))
                Next
                If Not bret Then
                    m_ErrorMsg = "Unsupported Format " + fileExtension
                End If
            Catch exp As System.Exception
                m_ErrorMsg = exp.Message
            End Try

            Return bret
        End Function
#End Region

#Region "DocumentCache"
        ''' <summary>
        ''' Remove cache files 
        ''' </summary>
        Private Sub RemoveCacheFiles()
            Try
                'ExStart:RemoveCacheFiles
                'Clear all cache files 
                'ExEnd:RemoveCacheFiles
                m_imgHandler.ClearCache()
            Catch exp As System.Exception
                Console.WriteLine(exp.Message)
            End Try
        End Sub
#End Region

    End Class

End Namespace

