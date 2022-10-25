
Imports GroupDocs.Viewer.Config
Imports GroupDocs.Viewer.Converter.Options
Imports GroupDocs.Viewer.Domain
Imports GroupDocs.Viewer.Domain.Options
Imports GroupDocs.Viewer.Handler
Imports System.Collections.Generic
Imports System.Drawing
Imports System.Drawing.Imaging
Imports System.IO
Imports System.Linq
Imports System.Text
Imports GroupDocs.Viewer.Domain.Containers
Imports GroupDocs.Viewer.Domain.Html
Imports GroupDocs.Viewer.Domain.Image
Imports Newtonsoft.Json
Imports Newtonsoft.Json.Linq
Imports System.Runtime.InteropServices

Namespace Nextlabs.GDViewer
    Public NotInheritable Class NxlFileHelper
        Private Enum CopyDataOpCode
            NxCmdLaunchNxrmc = 0
            NxCmdProtect = 1
            NxCmdUnprotect = 2
            NxCmdView = 3
            NxCmdShare = 4
            NxCmdProperties = 5
        End Enum
        Private Structure COPYDATASTRUCT
            Public dwData As IntPtr
            Public cbData As Integer
            Public lpData As IntPtr
        End Structure
        <DllImport("user32.dll", SetLastError:=True, CharSet:=CharSet.Auto)>
        Private Shared Function FindWindow(ByVal lpClassName As String, ByVal lpWindowName As String) As IntPtr
        End Function
        <DllImport("user32.dll", SetLastError:=True, CharSet:=CharSet.Unicode)>
        Private Shared Function SendMessageW(ByVal hwnd As IntPtr, ByVal wMsg As Integer, ByVal wParam As Integer, ByVal lParam As IntPtr) As Integer
        End Function
        Public Const tagViewerType As String = "viewerType"
        Public Const tagFilePath As String = "tempFilePath"
        Public Const tagFileShowPath As String = "filePath"
        Public Const tagFileOriginalName As String = "originalFileName"
        Public Const tagFileId As String = "fileId"
        Public Const tagFileUId As String = "fileUid"
        Public Const tagFileLastModifiedTime As String = "lastModifiedTime"
        Public Const tagFileSize As String = "fileSize"
        Public Const tagOfflineState As String = "offline"
        Public Const tagFavoriteState As String = "favorite"
        Public Const tagLocalFile As String = "local"
        Public Const tagEncryptedFlag As String = "tempFileEncrypted"

        Private Const WM_COPYDATA As Long = &H4A

        Public strFilePath As String
        Private strFileFullPath As String
        Public strFileShowPath As String
        Public strFileShowName As String
        Public strFileOriginalName As String
        Public strFileId As String
        Public strFileUId As String
        Public iFileSize As Integer
        Public dFileDate As DateTime
        Public bOffline As Boolean
        Public bFavorite As Boolean
        Public bEncrypted As Boolean
        Public strErr As String
        Public bLocal As Boolean
        Private Sub CleanData()
            iFileSize = 0
            bOffline = False
            bFavorite = False
            bEncrypted = False
            strFilePath = String.Empty
            strErr = String.Empty
        End Sub
        Public Sub New()
            CleanData()
        End Sub
        ''' <summary>
        '''  Allocate a pointer to an arbitrary structure on the global heap.
        ''' </summary>
        ''' <returns>pointer of the memory</returns>
        Private Function IntPtrAlloc(param) As IntPtr
            Dim retval As IntPtr = Marshal.AllocHGlobal(Marshal.SizeOf(param))
            Marshal.StructureToPtr(param, retval, False)
            Return retval
        End Function

        ''' <summary>
        ''' Free a pointer to an arbitrary structure from the global heap.
        ''' </summary>
        ''' <param name="preAllocated">pointer allocated by IntPtrAlloc function</param>
        Private Sub IntPtrFree(ByRef preAllocated As IntPtr)
            If (preAllocated = IntPtr.Zero) Then
                Return
            End If
            Marshal.FreeHGlobal(preAllocated)
            preAllocated = IntPtr.Zero
        End Sub

        Private Function SendMessageToRMC(msg As Integer, winhandle As IntPtr) As Boolean
            Dim rmcWindow As IntPtr = FindWindow("NXSD-NextLabsSkyDRM", Nothing)
            If rmcWindow = IntPtr.Zero Then
                strErr = "Please launch SkyDRM!"
                Return False
            End If
            Dim copyData As COPYDATASTRUCT = New COPYDATASTRUCT()
            copyData.dwData = msg
            copyData.lpData = Marshal.StringToHGlobalUni(strFileFullPath)
            copyData.cbData = Marshal.SizeOf(copyData.lpData)
            Dim copyDataBuff As IntPtr = IntPtrAlloc(copyData)
            SendMessageW(rmcWindow, WM_COPYDATA, winhandle, copyDataBuff)
            IntPtrFree(copyDataBuff)
            Marshal.FreeHGlobal(copyData.lpData)
            Return True
        End Function
        Public Function ShareFile(winhandle As IntPtr) As Boolean
            If (winhandle = 0) Then
                strErr = "Invalid Window Handle"
                Return False
            End If

            Return SendMessageToRMC(CopyDataOpCode.NxCmdShare, winhandle)
        End Function
        Public Function ProtectFile(winhandle As IntPtr) As Boolean
            If (winhandle = 0) Then
                strErr = "Invalid Window Handle"
                Return False
            End If
            Return SendMessageToRMC(CopyDataOpCode.NxCmdProtect, winhandle)
        End Function
        Public Function ShowFileInfo(winhandle As IntPtr) As Boolean
            If (winhandle = 0) Then
                strErr = "Invalid Window Handle"
                Return False
            End If
            Return SendMessageToRMC(CopyDataOpCode.NxCmdView, winhandle)
        End Function
        Public Function ParseVarible(ByVal type As String, ByVal params As String) As Boolean
            CleanData()
            If (type.CompareTo("Local") <> 0) Then
                strErr = "Invalid view type (" + type + ")"
                Return False
            End If
            Dim data() As Byte
            Dim base64Decoded As String
            Try
                data = System.Convert.FromBase64String(params)
                base64Decoded = System.Text.ASCIIEncoding.ASCII.GetString(data)
                ParseConfig(base64Decoded)
            Catch ex As Exception
                strErr = "Fail to decode parameters"
                Return False
            End Try

            Return True
        End Function
        Private Sub ParseConfig(ByVal JsonConfig As String)
            If (String.IsNullOrEmpty(JsonConfig)) Then
                strErr = "Invalid Json config" + JsonConfig
                Return
            End If
            Try
                Dim jsonObj As JObject = JObject.Parse(JsonConfig)
                Dim jData As List(Of JToken) = jsonObj.Children.ToList

                For Each item As JProperty In jData
                    item.CreateReader()
                    Select Case item.Name
                        Case tagViewerType
                            Continue For
                        Case tagFilePath
                            If (String.IsNullOrEmpty(item.Value.ToString())) Then
                                CleanData()
                                strErr = "Invalid file"
                                Return
                            End If
                            strFilePath = item.Value.ToString()
                        Case tagFileShowPath
                            If (String.IsNullOrEmpty(item.Value.ToString())) Then
                                CleanData()
                                strErr = "Invalid file Path"
                                Return
                            End If
                            strFileFullPath = item.Value.ToString()

                        Case tagFileOriginalName
                            If (String.IsNullOrEmpty(item.Value.ToString())) Then
                                CleanData()
                                strErr = "Invalid file Name"
                                Return
                            End If
                            strFileOriginalName = item.Value.ToString()
                        Case tagFileId
                            If (String.IsNullOrEmpty(item.Value.ToString())) Then
                                CleanData()
                                strErr = "Invalid file id"
                                Return
                            End If
                            strFileId = item.Value.ToString()

                        Case tagFileUId
                            If (String.IsNullOrEmpty(item.Value.ToString())) Then
                                CleanData()
                                strErr = "Invalid file Uid"
                                Return
                            End If
                            strFileUId = item.Value.ToString()
                        Case tagFileLastModifiedTime
                            If (String.IsNullOrEmpty(item.Value.ToString())) Then
                                CleanData()
                                strErr = "Invalid file date"
                                Return
                            End If
                            dFileDate = New DateTime(Convert.ToInt64(item.Value.ToString()))
                        Case tagFileSize
                            If (String.IsNullOrEmpty(item.Value.ToString())) Then
                                CleanData()
                                strErr = "Invalid file size"
                                Return
                            End If
                            iFileSize = Convert.ToInt32(item.Value.ToString())
                        Case tagOfflineState
                            If (String.IsNullOrEmpty(item.Value.ToString())) Then
                                bOffline = False
                                Continue For
                            End If
                            bOffline = Convert.ToBoolean(item.Value.ToString())
                        Case tagFavoriteState
                            If (String.IsNullOrEmpty(item.Value.ToString())) Then
                                bFavorite = False
                                Continue For
                            End If
                            bFavorite = Convert.ToBoolean(item.Value.ToString())
                        Case tagLocalFile
                            If (String.IsNullOrEmpty(item.Value.ToString())) Then
                                bLocal = False
                                Continue For
                            End If
                            bLocal = Convert.ToBoolean(item.Value.ToString())
                        Case tagEncryptedFlag
                            If (String.IsNullOrEmpty(item.Value.ToString())) Then
                                bEncrypted = False
                                Continue For
                            End If
                            bEncrypted = Convert.ToBoolean(item.Value.ToString())
                            If (bEncrypted) Then
                                CleanData()
                                strErr = "Can't handle the encrypted file!"
                                bEncrypted = True
                                Return
                            End If
                    End Select
                Next
                strFileShowPath = Path.GetDirectoryName(strFileFullPath)
                strFileShowName = Path.GetFileName(strFileFullPath)
                While (Path.GetExtension(strFileShowName).ToUpper.CompareTo(".NXL") = 0)
                    strFileShowName = strFileShowName.Remove(strFileShowName.Length - 4, 4)
                End While


            Catch ex As Exception
                strErr = ex.Message
                iFileSize = 0
                Return
            End Try
            Return
        End Sub
    End Class
End Namespace

Namespace GroupDocs.Viewer.Rendering
    Public NotInheritable Class Utilities
        Private Sub New()
        End Sub
        Public Const StoragePath As String = "\GDViewer\Data\Storage"
        Public Const CachePath As String = "\GDViewer\Data\Cache"
        Public Const OutputImagePath As String = "\GDViewer\Data\images"
        Public Const licensePath As String = "GroupDocs.Viewer.lic"
#Region "Others"
        ''' <summary>
        ''' Get all supported document formats
        ''' </summary>

        Public Shared Sub ShowAllSupportedFormats()
            ' Setup GroupDocs.Viewer config
            Dim config As ViewerConfig = Utilities.GetConfigurations()

            ' Create image or html handler
            Dim imageHandler As New ViewerImageHandler(config)

            ' Get supported document formats
            Dim documentFormatsContainer As DocumentFormatsContainer = imageHandler.GetSupportedDocumentFormats()
            Dim supportedDocumentFormats As Dictionary(Of String, String) = documentFormatsContainer.SupportedDocumentFormats

            For Each supportedDocumentFormat As KeyValuePair(Of String, String) In supportedDocumentFormats
                Console.WriteLine(String.Format("Extension: '{0}'; Document format: '{1}'", supportedDocumentFormat.Key, supportedDocumentFormat.Value))
            Next
        End Sub
#End Region
#Region "Configurations"
        ''' <summary>
        ''' Initialize, populate and return the ViewerConfig object
        ''' </summary>
        ''' <returns>Populated ViewerConfig Object</returns>
        Public Shared Function GetConfigurations() As ViewerConfig
            'ExStart:Configurations
            Dim config As New ViewerConfig()
            'set the storage path
            config.StoragePath = Path.GetTempPath + StoragePath
            If (Not System.IO.Directory.Exists(config.StoragePath)) Then
                System.IO.Directory.CreateDirectory(config.StoragePath)
            End If

            config.UseCache = False
            Return config
            'ExEnd:Configurations

        End Function

        ''' <summary>
        ''' Initialize, populate and return the ViewerConfig object
        ''' </summary>
        ''' <param name="DefaultFontName">Font Name</param>
        ''' <returns>Populated ViewerConfig Object</returns>
        Public Shared Function GetConfigurations(DefaultFontName As String) As ViewerConfig
            'ExStart:ConfigurationsWithDefaultFontName
            Dim config As ViewerConfig = GetConfigurations()
            'Set default font name
            config.DefaultFontName = DefaultFontName
            Return config
            'ExEnd:ConfigurationsWithDefaultFontName

        End Function

        Public Shared Function GetOptionByType(doctype As DocumentType, Optional docPassword As String = Nothing) As ImageOptions
            Dim options As New ImageOptions()

            Select Case doctype
                Case DocumentType.OfficeExcel
                    options.CellsOptions.OnePagePerSheet = False
                    options.CellsOptions.ShowGridLines = True
                    options.CellsOptions.CountRowsPerPage = 50
                Case Else
            End Select

            ' Set password if document is password protected. 
            If Not [String].IsNullOrEmpty(docPassword) Then
                options.Password = docPassword
            End If
            Return options
        End Function


#End Region

#Region "Transformations"

        Public NotInheritable Class PageTransformations
            Private Sub New()
            End Sub

            ''' <summary>
            '''  Rotate a Page before rendering image
            ''' </summary>
            ''' <param name="handler"></param>
            ''' <param name="guid"></param>
            ''' <param name="PageNumber"></param>
            ''' <param name="angle"></param>
            ''' <remarks></remarks>
            Public Shared Sub RotatePages(ByRef handler As ViewerHandler(Of PageImage), guid As [String], PageNumber As Integer, angle As Integer)
                'ExStart:rotationAngle
                ' Set the property of handler's rotate Page
                handler.RotatePage(guid, New RotatePageOptions(PageNumber, angle))
                'ExEnd:rotationAngle
            End Sub


            ''' <summary>
            ''' add a watermark text to all rendered images.
            ''' </summary>
            ''' <param name="options">HtmlOptions by reference</param>
            ''' <param name="text">Watermark text</param>
            ''' <param name="color">System.Drawing.Color</param>
            ''' <param name="position"></param>
            ''' <param name="width"></param>
            Public Shared Sub AddWatermark(ByRef options As ImageOptions, text As [String], color As Color, position As WatermarkPosition, width As Integer)
                'ExStart:AddWatermark
                'Initialize watermark object by passing the text to display.
                Dim watermark As New Watermark(text)
                'Apply the watermark color by assigning System.Drawing.Color.
                watermark.Color = color
                'Set the watermark's position by assigning an enum WatermarkPosition's value.
                watermark.Position = position
                'set an integer value as watermark width 
                watermark.Width = width
                ' Set font name
                watermark.FontName = "MS Gothic"
                'Assign intialized and populated watermark object to ImageOptions or HtmlOptions objects
                options.Watermark = watermark
                'ExEnd:AddWatermark
            End Sub
            ''' <summary>
            ''' add a watermark text to all rendered Html pages.
            ''' </summary>
            ''' <param name="options">HtmlOptions by reference</param>
            ''' <param name="text">Watermark text</param>
            ''' <param name="color">System.Drawing.Color</param>
            ''' <param name="position"></param>
            ''' <param name="width"></param>
            Public Shared Sub AddWatermark(ByRef options As HtmlOptions, text As [String], color As Color, position As WatermarkPosition, width As Integer)

                Dim watermark As New Watermark(text)
                watermark.Color = color
                watermark.Position = position
                watermark.Width = width
                watermark.FontName = """Comic Sans MS"", cursive, sans-serif"
                options.Watermark = watermark
            End Sub

        End Class

#End Region

#Region "ProductLicense"

        ''' <summary>
        ''' Set product's license for HTML Handler
        ''' </summary>
        Public Shared Sub ApplyLicense()
            Dim lic As New Global.GroupDocs.Viewer.License()
            lic.SetLicense(licensePath)
        End Sub

#End Region

#Region "OutputHandling"
        ''' <summary>
        ''' Save the rendered images at disk
        ''' </summary>
        ''' <param name="imageName">Save as provided string</param>
        ''' <param name="destPath">Save as provided string</param>
        ''' <param name="imageContent">stream of image contents</param>
        Public Shared Function SaveAsImage(imageName As [String], destPath As [String], imageContent As Stream) As String
            Try
                If Not System.IO.Directory.Exists(destPath) Then
                    System.IO.Directory.CreateDirectory(destPath)
                End If
                Dim filename As String = Path.Combine(Path.GetFullPath(destPath), Path.GetFileNameWithoutExtension(imageName)) + ".Jpeg"
                'ExStart:SaveAsImage
                ' extract the image from stream
                Dim img As Image = Image.FromStream(imageContent)
                'save the image in the form of jpeg
                'ExEnd:SaveAsImage
                img.Save(filename, ImageFormat.Jpeg)
                Return filename
            Catch ex As System.Exception
                Console.WriteLine(ex.Message)
                Return ""
            End Try
        End Function
        ''' <summary>
        ''' Get document stream
        ''' </summary>
        ''' <param name="DocumentName">Input document name</param> 
        Public Shared Function GetDocumentStream(DocumentName As String) As Stream
            Try
                'ExStart:GetDocumentStream
                Dim fsSource As New FileStream(My.Application.Info.DirectoryPath & DocumentName, FileMode.Open, FileAccess.Read)

                ' Read the source file into a byte array.
                Dim bytes As Byte() = New Byte(fsSource.Length - 1) {}
                Dim numBytesToRead As Integer = CInt(fsSource.Length)
                Dim numBytesRead As Integer = 0
                While numBytesToRead > 0
                    ' Read may return anything from 0 to numBytesToRead.
                    Dim n As Integer = fsSource.Read(bytes, numBytesRead, numBytesToRead)

                    ' Break when the end of the file is reached.
                    If n = 0 Then
                        Exit While
                    End If

                    numBytesRead += n
                    numBytesToRead -= n
                End While
                numBytesToRead = bytes.Length



                'ExEnd:GetDocumentStream
                Return fsSource
            Catch ioEx As FileNotFoundException
                Console.WriteLine(ioEx.Message)
                Return Nothing
            End Try
        End Function
#End Region
    End Class

End Namespace

