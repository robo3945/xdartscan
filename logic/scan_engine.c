#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <time.h>
#include <sys/stat.h>
#include "../headers/scan_engine.h"
#include "../headers/config.h"
#include "../headers/config_manager.h"
#include "../headers/random_test.h"
#include "../headers/report_manager.h"

static const int MAGIC_NUMBER_BYTE_SIZE = 4;

bool p_binary_search(unsigned long magic_number, int lower, int upper);

void p_scan_file(char *basePath, bool verbose);

void p_scan_files(char *base_path, int indent, bool verbose);

long p_read_length(FILE *fp);

MagicNumber g_well_known_mn[] = {
        {"41435344",                                                                                             "[['*', 'AOL parameter|info files']]"},
        {"62706c697374",                                                                                         "[['*', 'Binary property list (plist)']]"},
        {"001400000102",                                                                                         "[['*', 'BIOS details in RAM']]"},
        {"3037303730",                                                                                           "[['*', 'cpio archive']]"},
        {"7f454c46",                                                                                             "[['*', 'ELF executable']]"},
        {"a1b2cd34",                                                                                             "[['*', 'Extended tcpdump (libpcap) capture file']]"},
        {"04000000",                                                                                             "[['*', 'INFO2 Windows recycle bin_1']]"},
        {"05000000",                                                                                             "[['*', 'INFO2 Windows recycle bin_2']]"},
        {"aced",                                                                                                 "[['*', 'Java serialization data']]"},
        {"4b57414a88f027d1",                                                                                     "[['*', 'KWAJ (compressed) file']]"},
        {"cd20aaaa02000000",                                                                                     "[['*', 'NAV quarantined virus file']]"},
        {"535a2088f02733d1",                                                                                     "[['*', 'QBASIC SZDD file']]"},
        {"6f3c",                                                                                                 "[['*', 'SMS text (SIM)']]"},
        {"535a444488f02733",                                                                                     "[['*', 'SZDD file format']]"},
        {"a1b2c3d4",                                                                                             "[['*', 'tcpdump (libpcap) capture file']]"},
        {"34cdb2a1",                                                                                             "[['*', 'Tcpdump capture file']]"},
        {"efbbbf",                                                                                               "[['*', 'UTF8 file']]"},
        {"feff",                                                                                                 "[['*', 'UTF-16|UCS-2 file']]"},
        {"fffe0000",                                                                                             "[['*', 'UTF-32|UCS-4 file']]"},
        {"626567696e",                                                                                           "[['*', 'UUencoded file']]"},
        {"d4c3b2a1",                                                                                             "[['*', 'WinDump (winpcap) capture file']]"},
        {"37e45396c9dbd607",                                                                                     "[['*', 'zisofs compressed file']]"},
        {"00001a00051004",                                                                                       "[['123', 'Lotus 1-2-3 (v9)']]"},
        {"4d5a",                                                                                                 "[['386', 'Windows virtual device drivers'], ['acm', 'MS audio compression manager driver'], ['ax', 'Library cache file'], ['com', 'Windows|DOS executable file'], ['cpl', 'Control panel application'], ['dll', 'Windows|DOS executable file'], ['drv', 'Windows|DOS executable file'], ['exe', 'Windows|DOS executable file'], ['fon', 'Font file'], ['ocx', 'ActiveX|OLE Custom Control'], ['olb', 'OLE object library'], ['pif', 'Windows|DOS executable file'], ['qts', 'Windows|DOS executable file'], ['qtx', 'Windows|DOS executable file'], ['scr', 'Screen saver'], ['sys', 'Windows|DOS executable file'], ['vbx', 'VisualBASIC application'], ['vxd', 'Windows virtual device drivers']]"},
        {"0000001466747970",                                                                                     "[['3gp', '3GPP multimedia files']]"},
        {"0000002066747970",                                                                                     "[['3gp', '3GPP2 multimedia files']]"},
        {"0000001866747970",                                                                                     "[['3gp5', 'MPEG-4 video files']]"},
        {"52494646",                                                                                             "[['4xm', '4X Movie video'], ['ani', 'Windows animated cursor'], ['avi', 'Resource Interchange File Format'], ['cda', 'Resource Interchange File Format'], ['cdr', 'CorelDraw document'], ['cmx', 'Corel Presentation Exchange metadata'], ['dat', 'Video CD MPEG movie'], ['dat', 'Video CD MPEG movie'], ['ds4', 'Micrografx Designer graphic'], ['qcp', 'Resource Interchange File Format'], ['rmi', 'Resource Interchange File Format'], ['wav', 'Resource Interchange File Format']]"},
        {"377abcaf271c",                                                                                         "[['7z', '7-Zip compressed file']]"},
        {"00014241",                                                                                             "[['aba', 'Palm Address Book Archive']]"},
        {"5157205665722e20",                                                                                     "[['abd', 'ABD | QSD Quicken data file'], ['qsd', 'ABD | QSD Quicken data file']]"},
        {"414f4c494e444558",                                                                                     "[['abi', 'AOL address book index']]"},
        {"414f4c",                                                                                               "[['abi', 'AOL config files'], ['aby', 'AOL config files'], ['bag', 'AOL config files'], ['idx', 'AOL config files'], ['ind', 'AOL config files'], ['pfc', 'AOL config files']]"},
        {"414f4c4442",                                                                                           "[['aby', 'AOL address book'], ['idx', 'AOL user configuration']]"},
        {"72696666",                                                                                             "[['ac', 'Sonic Foundry Acid Music File']]"},
        {"000100005374616e6461726420414345204442",                                                               "[['accdb', 'Microsoft Access 2007']]"},
        {"c3abcdab",                                                                                             "[['acs', 'MS Agent Character file']]"},
        {"d0cf11e0a1b11ae1",                                                                                     "[['ac_', 'CaseWare Working Papers'], ['adp', 'Access project file'], ['apr', 'Lotus|IBM Approach 97 file'], ['db', 'MSWorks database file'], ['doc', 'Microsoft Office document'], ['dot', 'Microsoft Office document'], ['msc', 'Microsoft Common Console Document'], ['msi', 'Microsoft Installer package'], ['mtw', 'Minitab data file'], ['opt', 'Developer Studio File Options file'], ['pps', 'Microsoft Office document'], ['ppt', 'Microsoft Office document'], ['pub', 'MS Publisher file'], ['rvt', 'Revit Project file'], ['sou', 'Visual Studio Solution User Options file'], ['spo', 'SPSS output file'], ['vsd', 'Visio file'], ['wiz', 'Microsoft Office document'], ['wps', 'MSWorks text document'], ['xla', 'Microsoft Office document'], ['xls', 'Microsoft Office document']]"},
        {"5245564e554d3a2c",                                                                                     "[['ad', 'Antenna data file']]"},
        {"444f53",                                                                                               "[['adf', 'Amiga disk file']]"},
        {"0300000041505052",                                                                                     "[['adx', 'Approach index file']]"},
        {"80000020031204",                                                                                       "[['adx', 'Dreamcast audio']]"},
        {"464f524d00",                                                                                           "[['aiff', 'Audio Interchange File']]"},
        {"2112",                                                                                                 "[['ain', 'AIN Compressed Archive']]"},
        {"2321414d52",                                                                                           "[['amr', 'Adaptive Multi-Rate ACELP Codec (GSM)']]"},
        {"4d5a900003000000",                                                                                     "[['api', 'Acrobat plug-in'], ['ax', 'DirectShow filter'], ['flt', 'Audition graphic filter']]"},
        {"41724301",                                                                                             "[['arc', 'FreeArc compressed file']]"},
        {"1a02",                                                                                                 "[['arc', 'LH archive (old vers.|type 1)']]"},
        {"1a03",                                                                                                 "[['arc', 'LH archive (old vers.|type 2)']]"},
        {"1a04",                                                                                                 "[['arc', 'LH archive (old vers.|type 3)']]"},
        {"1a08",                                                                                                 "[['arc', 'LH archive (old vers.|type 4)']]"},
        {"1a09",                                                                                                 "[['arc', 'LH archive (old vers.|type 5)']]"},
        {"60ea",                                                                                                 "[['arj', 'ARJ Compressed archive file']]"},
        {"d42a",                                                                                                 "[['arl', 'AOL history|typed URL files'], ['aut', 'AOL history|typed URL files']]"},
        {"3026b2758e66cf11",                                                                                     "[['asf', 'Windows Media Audio|Video File'], ['wma', 'Windows Media Audio|Video File'], ['wmv', 'Windows Media Audio|Video File']]"},
        {"5343486c",                                                                                             "[['ast', 'Underground Audio']]"},
        {"3c",                                                                                                   "[['asx', 'Advanced Stream Redirector'], ['xdr', 'BizTalk XML-Data Reduced Schema']]"},
        {"646e732e",                                                                                             "[['au', 'Audacity audio file']]"},
        {"2e736e64",                                                                                             "[['au', 'NeXT|Sun Microsystems audio file']]"},
        {"8a0109000000e108",                                                                                     "[['aw', 'MS Answer Wizard']]"},
        {"414f4c2046656564",                                                                                     "[['bag', 'AOL and AIM buddy list']]"},
        {"5854",                                                                                                 "[['bdr', 'MS Publisher']]"},
        {"424c4932323351",                                                                                       "[['bin', 'Speedtouch router firmware']]"},
        {"424d",                                                                                                 "[['bmp', 'Bitmap image'], ['dib', 'Bitmap image']]"},
        {"425a68",                                                                                               "[['bz2', 'bzip2 compressed archive'], ['tar.bz2', 'bzip2 compressed archive'], ['tb2', 'bzip2 compressed archive'], ['tbz2', 'bzip2 compressed archive']]"},
        {"49536328",                                                                                             "[['cab', 'Install Shield compressed file'], ['hdr', 'Install Shield compressed file']]"},
        {"4d534346",                                                                                             "[['cab', 'Microsoft cabinet file'], ['ppz', 'Powerpoint Packaged Presentation'], ['snp', 'MS Access Snapshot Viewer file']]"},
        {"737263646f636964",                                                                                     "[['cal', 'CALS raster bitmap']]"},
        {"537570657243616c",                                                                                     "[['cal', 'SuperCalc worksheet']]"},
        {"b5a2b0b3b3b0a5b5",                                                                                     "[['cal', 'Windows calendar']]"},
        {"58435000",                                                                                             "[['cap', 'Packet sniffer files']]"},
        {"52545353",                                                                                             "[['cap', 'WinNT Netmon capture file']]"},
        {"5f434153455f",                                                                                         "[['cas', 'EnCase case file'], ['cbk', 'EnCase case file']]"},
        {"30",                                                                                                   "[['cat', 'MS security catalog file']]"},
        {"434246494c45",                                                                                         "[['cbd', 'WordPerfect dictionary']]"},
        {"454c49544520436f",                                                                                     "[['cdr', 'Elite Plus Commander game file']]"},
        {"4d535f564f494345",                                                                                     "[['cdr', 'Sony Compressed Voice File'], ['dvf', 'Sony Compressed Voice File'], ['msv', 'Sony Compressed Voice File']]"},
        {"5b666c7473696d2e",                                                                                     "[['cfg', 'Flight Simulator Aircraft Configuration']]"},
        {"49545346",                                                                                             "[['chi', 'MS Compiled HTML Help File'], ['chm', 'MS Compiled HTML Help File']]"},
        {"cafebabe",                                                                                             "[['class', 'Java bytecode']]"},
        {"434f4d2b",                                                                                             "[['clb', 'COM+ Catalog']]"},
        {"434d5831",                                                                                             "[['clb', 'Corel Binary metafile']]"},
        {"53514c4f434f4e56",                                                                                     "[['cnv', 'DB2 conversion file']]"},
        {"4e616d653a20",                                                                                         "[['cod', 'Agent newsreader character map']]"},
        {"e8",                                                                                                   "[['com', 'Windows executable file_1'], ['sys', 'Windows executable file_1']]"},
        {"e9",                                                                                                   "[['com', 'Windows executable file_2'], ['sys', 'Windows executable file_2']]"},
        {"eb",                                                                                                   "[['com', 'Windows executable file_3'], ['sys', 'Windows executable file_3']]"},
        {"464158434f564552",                                                                                     "[['cpe', 'MS Fax Cover Sheet']]"},
        {"53494554524f4e49",                                                                                     "[['cpi', 'Sietronics CPI XRD document']]"},
        {"ff464f4e54",                                                                                           "[['cpi', 'Windows international code page']]"},
        {"dcdc",                                                                                                 "[['cpl', 'Corel color palette']]"},
        {"4350543746494c45",                                                                                     "[['cpt', 'Corel Photopaint file_1']]"},
        {"43505446494c45",                                                                                       "[['cpt', 'Corel Photopaint file_2']]"},
        {"5b57696e646f7773",                                                                                     "[['cpx', 'Microsoft Code Page Translation file']]"},
        {"43525553482076",                                                                                       "[['cru', 'Crush compressed archive']]"},
        {"49491a0000004845",                                                                                     "[['crw', 'Canon RAW file']]"},
        {"6375736800000002",                                                                                     "[['csh', 'Photoshop Custom Shape']]"},
        {"436174616c6f6720",                                                                                     "[['ctf', 'WhereIsIt Catalog']]"},
        {"56455253494f4e20",                                                                                     "[['ctl', 'Visual Basic User-defined Control file']]"},
        {"504b0304",                                                                                             "[['cuix', 'Customization files'], ['docx', 'MS Office Open XML Format Document'], ['jar', 'Java archive_1'], ['kwd', 'KWord document'], ['odp', 'OpenDocument template'], ['odt', 'OpenDocument template'], ['ott', 'OpenDocument template'], ['pptx', 'MS Office Open XML Format Document'], ['sxc', 'StarOffice spreadsheet'], ['sxd', 'OpenOffice documents'], ['sxi', 'OpenOffice documents'], ['sxw', 'OpenOffice documents'], ['wmz', 'Windows Media compressed skin file'], ['xlsx', 'MS Office Open XML Format Document'], ['xpi', 'Mozilla Browser Archive'], ['xps', 'XML paper specification file'], ['xpt', 'eXact Packager Models'], ['zip', 'PKZIP archive_1']]"},
        {"00000200",                                                                                             "[['cur', 'Windows cursor'], ['wb2', 'QuattroPro spreadsheet']]"},
        {"a90d000000000000",                                                                                     "[['dat', 'Access Data FTK evidence'], ['dat', 'Access Data FTK evidence']]"},
        {"736c6821",                                                                                             "[['dat', 'Allegro Generic Packfile (compressed)']]"},
        {"496e6e6f20536574",                                                                                     "[['dat', 'Inno Setup Uninstall Log']]"},
        {"4552465353415645",                                                                                     "[['dat', 'EasyRecovery Saved State file']]"},
        {"415647365f496e74",                                                                                     "[['dat', 'AVG6 Integrity database']]"},
        {"504e4349554e444f",                                                                                     "[['dat', 'Norton Disk Doctor undo file']]"},
        {"03",                                                                                                   "[['dat', 'MapInfo Native Data Format'], ['db3', 'dBASE III file']]"},
        {"55464f4f72626974",                                                                                     "[['dat', 'UFO Capture map file']]"},
        {"436c69656e742055",                                                                                     "[['dat', 'IE History file']]"},
        {"50455354",                                                                                             "[['dat', 'PestPatrol data|main_scan strings']]"},
        {"4e41565452414646",                                                                                     "[['dat', 'TomTom traffic data']]"},
        {"43524547",                                                                                             "[['dat', 'Win9x registry hive']]"},
        {"52415a4154444231",                                                                                     "[['dat', 'Shareaza (P2P) thumbnail']]"},
        {"72656766",                                                                                             "[['dat', 'WinNT registry file']]"},
        {"1a52545320434f4d",                                                                                     "[['dat', 'Runtime Software disk image']]"},
        {"44424648",                                                                                             "[['db', 'Palm Zire photo database']]"},
        {"fdffffff",                                                                                             "[['db', 'Thumbs.db subheader']]"},
        {"0006156100000002000004d200001000",                                                                     "[['db', 'Netscape Navigator (v4) database']]"},
        {"08",                                                                                                   "[['db', 'dBASE IV or dBFast configuration file']]"},
        {"53514c69746520666f726d6174203300",                                                                     "[['db', 'SQLite database file']]"},
        {"04",                                                                                                   "[['db4', 'dBASE IV file']]"},
        {"00014244",                                                                                             "[['dba', 'Palm DateBook Archive']]"},
        {"6c33336c",                                                                                             "[['dbb', 'Skype user data file']]"},
        {"4f504c4461746162",                                                                                     "[['dbf', 'Psion Series 3 Database']]"},
        {"cfad12fe",                                                                                             "[['dbx', 'Outlook Express e-mail folder']]"},
        {"3c21646f63747970",                                                                                     "[['dci', 'AOL HTML mail']]"},
        {"b168de3a",                                                                                             "[['dcx', 'PCX bitmap']]"},
        {"6465780a30303900",                                                                                     "[['dex', 'Dalvik (Android) executable file']]"},
        {"78",                                                                                                   "[['dmg', 'MacOS X image file']]"},
        {"4d444d5093a7",                                                                                         "[['dmp', 'Windows dump file'], ['hdmp', 'Windows dump file']]"},
        {"504147454455",                                                                                         "[['dmp', 'Windows memory dump']]"},
        {"444d5321",                                                                                             "[['dms', 'Amiga DiskMasher compressed archive']]"},
        {"0d444f43",                                                                                             "[['doc', 'DeskMate Document']]"},
        {"cf11e0a1b11ae100",                                                                                     "[['doc', 'Perfect Office document']]"},
        {"dba52d00",                                                                                             "[['doc', 'Word 2.0 file']]"},
        {"eca5c100",                                                                                             "[['doc', 'Word document subheader']]"},
        {"504b030414000600",                                                                                     "[['docx', 'MS Office 2007 documents'], ['pptx', 'MS Office 2007 documents'], ['xlsx', 'MS Office 2007 documents']]"},
        {"07",                                                                                                   "[['drw', 'Generic drawing programs']]"},
        {"01ff02040302",                                                                                         "[['drw', 'Micrografx vector graphic file']]"},
        {"4d56",                                                                                                 "[['dsn', 'CD Stomper Pro label file']]"},
        {"23204d6963726f73",                                                                                     "[['dsp', 'MS Developer Studio project file']]"},
        {"02647373",                                                                                             "[['dss', 'Digital Speech Standard file']]"},
        {"64737766696c65",                                                                                       "[['dsw', 'MS Visual Studio workspace file']]"},
        {"0764743264647464",                                                                                     "[['dtd', 'DesignTools 2D Design file']]"},
        {"5b50686f6e655d",                                                                                       "[['dun', 'Dial-up networking file']]"},
        {"445644",                                                                                               "[['dvr', 'DVR-Studio stream file'], ['ifo', 'DVD info file']]"},
        {"4f7b",                                                                                                 "[['dw4', 'Visio|DisplayWrite 4 text file']]"},
        {"41433130",                                                                                             "[['dwg', 'Generic AutoCAD drawing']]"},
        {"455646090d0aff00",                                                                                     "[['e01', 'Expert Witness Compression Format']]"},
        {"4c5646090d0aff00",                                                                                     "[['e01', 'Logical File Evidence Format']]"},
        {"5b47656e6572616c",                                                                                     "[['ecf', 'MS Exchange configuration file']]"},
        {"dcfe",                                                                                                 "[['efx', 'eFax file']]"},
        {"582d",                                                                                                 "[['eml', 'Exchange e-mail']]"},
        {"52657475726e2d50",                                                                                     "[['eml', 'Generic e-mail_1']]"},
        {"46726f6d",                                                                                             "[['eml', 'Generic e-mail_2']]"},
        {"40404020000040404040",                                                                                 "[['enl', 'EndNote Library File']]"},
        {"c5d0d3c6",                                                                                             "[['eps', 'Adobe encapsulated PostScript']]"},
        {"252150532d41646f",                                                                                     "[['eps', 'Encapsulated PostScript file']]"},
        {"1a350100",                                                                                             "[['eth', 'WinPharoah capture file']]"},
        {"300000004c664c65",                                                                                     "[['evt', 'Windows Event Viewer file']]"},
        {"456c6646696c6500",                                                                                     "[['evtx', 'Windows Vista event log']]"},
        {"25504446",                                                                                             "[['fdf', 'PDF file'], ['pdf', 'PDF file']]"},
        {"664c614300000022",                                                                                     "[['flac', 'Free Lossless Audio Codec file']]"},
        {"0011",                                                                                                 "[['fli', 'FLIC animation']]"},
        {"76323030332e3130",                                                                                     "[['flt', 'Qimage filter']]"},
        {"464c56",                                                                                               "[['flv', 'Flash video file']]"},
        {"3c4d616b65724669",                                                                                     "[['fm', 'Adobe FrameMaker'], ['mif', 'Adobe FrameMaker']]"},
        {"d20a0000",                                                                                             "[['ftr', 'WinPharoah filter file']]"},
        {"feef",                                                                                                 "[['gho', 'Symantex Ghost image file'], ['ghs', 'Symantex Ghost image file']]"},
        {"3f5f0300",                                                                                             "[['gid', 'Windows Help file_2'], ['hlp', 'Windows Help file_2']]"},
        {"4c4e0200",                                                                                             "[['gid', 'Windows help file_3'], ['hlp', 'Windows help file_3']]"},
        {"47494638",                                                                                             "[['gif', 'GIF file']]"},
        {"99",                                                                                                   "[['gpg', 'GPG public keyring']]"},
        {"504d4343",                                                                                             "[['grp', 'Windows Program Manager group file']]"},
        {"475832",                                                                                               "[['gx2', 'Show Partner graphics file']]"},
        {"1f8b08",                                                                                               "[['gz', 'GZIP archive file']]"},
        {"91334846",                                                                                             "[['hap', 'Hamarsoft compressed archive']]"},
        {"233f52414449414e",                                                                                     "[['hdr', 'Radiance High Dynamic Range image file']]"},
        {"48695021",                                                                                             "[['hip', 'Houdini image file. Three-dimensional modeling and animation']]"},
        {"0000ffffffff",                                                                                         "[['hlp', 'Windows Help file_1']]"},
        {"2854686973206669",                                                                                     "[['hqx', 'BinHex 4 Compressed Archive']]"},
        {"00000100",                                                                                             "[['ico', 'Windows icon|printer spool file'], ['spl', 'Windows icon|printer spool file']]"},
        {"5000000020000000",                                                                                     "[['idx', 'Quicken QuickFinder Information File']]"},
        {"504943540008",                                                                                         "[['img', 'ChromaGraph Graphics Card Bitmap']]"},
        {"eb3c902a",                                                                                             "[['img', 'GEM Raster file']]"},
        {"53434d49",                                                                                             "[['img', 'Img Software Bitmap']]"},
        {"414f4c494458",                                                                                         "[['ind', 'AOL client preferences|settings file']]"},
        {"e310000100000000",                                                                                     "[['info', 'Amiga icon']]"},
        {"5468697320697320",                                                                                     "[['info', 'GNU Info Reader file']]"},
        {"7a626578",                                                                                             "[['info', 'ZoomBrowser Image Index']]"},
        {"4344303031",                                                                                           "[['iso', 'ISO-9660 CD Disc Image']]"},
        {"2e524543",                                                                                             "[['ivr', 'RealPlayer video file (V11+)']]"},
        {"5f27a889",                                                                                             "[['jar', 'Jar archive']]"},
        {"4a4152435300",                                                                                         "[['jar', 'JARCS compressed archive']]"},
        {"504b030414000800",                                                                                     "[['jar', 'Java archive_2']]"},
        {"ffd8ffe0",                                                                                             "[['jfif', 'JPEG IMAGE'], ['jfif', 'JFIF IMAGE FILE - jpeg'], ['jpe', 'JPEG IMAGE'], ['jpe', 'JPE IMAGE FILE - jpeg'], ['jpeg', 'JPEG IMAGE'], ['jpg', 'JPEG IMAGE']]"},
        {"4a47030e",                                                                                             "[['jg', 'AOL ART file_1']]"},
        {"4a47040e",                                                                                             "[['jg', 'AOL ART file_2']]"},
        {"4e422a00",                                                                                             "[['jnt', 'MS Windows journal'], ['jtp', 'MS Windows journal']]"},
        {"0000000c6a502020",                                                                                     "[['jp2', 'JPEG2000 image files']]"},
        {"ffd8ffe2",                                                                                             "[['jpeg', 'CANNON EOS JPEG FILE']]"},
        {"ffd8ffe3",                                                                                             "[['jpeg', 'SAMSUNG D500 JPEG FILE']]"},
        {"ffd8ffe1",                                                                                             "[['jpg', 'Digital camera JPG using Exchangeable Image File Format (EXIF)']]"},
        {"ffd8ffe8",                                                                                             "[['jpg', 'Still Picture Interchange File Format (SPIFF)']]"},
        {"4b47425f61726368",                                                                                     "[['kgb', 'KGB archive']]"},
        {"49443303000000",                                                                                       "[['koz', 'Sprint Music Store audio']]"},
        {"c8007900",                                                                                             "[['lbk', 'Jeppesen FliteLog file']]"},
        {"7b0d0a6f20",                                                                                           "[['lgc', 'Windows application log'], ['lgd', 'Windows application log']]"},
        {"2d6c68",                                                                                               "[['lha', 'Compressed archive'], ['lzh', 'Compressed archive']]"},
        {"213c617263683e0a",                                                                                     "[['lib', 'Unix archiver (ar)|MS Program Library Common Object File Format (COFF)']]"},
        {"49544f4c49544c53",                                                                                     "[['lit', 'MS Reader eBook']]"},
        {"4c00000001140200",                                                                                     "[['lnk', 'Windows shortcut file']]"},
        {"2a2a2a2020496e73",                                                                                     "[['log', 'Symantec Wise Installer log']]"},
        {"576f726450726f",                                                                                       "[['lwp', 'Lotus WordPro file']]"},
        {"00000020667479704d3441",                                                                               "[['m4a', 'Apple audio and video files']]"},
        {"3c3f786d6c2076657273696f6e3d",                                                                         "[['manifest', 'Windows Visual Stylesheet']]"},
        {"4d41723000",                                                                                           "[['mar', 'MAr compressed archive']]"},
        {"4d415243",                                                                                             "[['mar', 'Microsoft|MSN MARC archive']]"},
        {"4d41523100",                                                                                           "[['mar', 'Mozilla archive']]"},
        {"000100005374616e64617264204a6574204442",                                                               "[['mdb', 'Microsoft Access']]"},
        {"010f0000",                                                                                             "[['mdf', 'SQL Data Base']]"},
        {"4550",                                                                                                 "[['mdi', 'MS Document Imaging file']]"},
        {"4d546864",                                                                                             "[['mid', 'MIDI sound file'], ['midi', 'MIDI sound file']]"},
        {"56657273696f6e20",                                                                                     "[['mif', 'MapInfo Interchange Format file']]"},
        {"1a45dfa393428288",                                                                                     "[['mkv', 'Matroska stream file']]"},
        {"4d494c4553",                                                                                           "[['mls', 'Milestones project management file']]"},
        {"4d56323134",                                                                                           "[['mls', 'Milestones project management file_1']]"},
        {"4d563243",                                                                                             "[['mls', 'Milestones project management file_2']]"},
        {"4d4c5357",                                                                                             "[['mls', 'Skype localization data file']]"},
        {"4d4d4d440000",                                                                                         "[['mmf', 'Yamaha Synthetic music Mobile Application Format']]"},
        {"000100004d534953414d204461746162617365",                                                               "[['mny', 'Microsoft Money file']]"},
        {"fffe23006c006900",                                                                                     "[['mof', 'MSinfo file']]"},
        {"6d6f6f76",                                                                                             "[['mov', 'QuickTime movie_1']]"},
        {"66726565",                                                                                             "[['mov', 'QuickTime movie_2']]"},
        {"6d646174",                                                                                             "[['mov', 'QuickTime movie_3']]"},
        {"77696465",                                                                                             "[['mov', 'QuickTime movie_4']]"},
        {"706e6f74",                                                                                             "[['mov', 'QuickTime movie_5']]"},
        {"736b6970",                                                                                             "[['mov', 'QuickTime movie_6']]"},
        {"0ced",                                                                                                 "[['mp', 'Monochrome Picture TIFF bitmap']]"},
        {"494433",                                                                                               "[['mp3', 'MP3 audio file']]"},
        {"000001ba",                                                                                             "[['mpg', 'DVD video file'], ['vob', 'DVD video file']]"},
        {"000001b3",                                                                                             "[['mpg', 'MPEG video file']]"},
        {"3c3f786d6c2076657273696f6e3d22312e30223f3e0d0a3c4d4d435f436f6e736f6c6546696c6520436f6e736f6c65566572", "[['msc', 'MMC Snap-in Control file']]"},
        {"2320",                                                                                                 "[['msi', 'Cerius2 file']]"},
        {"0e4e65726f49534f",                                                                                     "[['nri', 'Nero CD compilation']]"},
        {"1a0000040000",                                                                                         "[['nsf', 'Lotus Notes database']]"},
        {"4e45534d1a01",                                                                                         "[['nsf', 'NES Sound file']]"},
        {"1a0000",                                                                                               "[['ntf', 'Lotus Notes database template']]"},
        {"4e49544630",                                                                                           "[['ntf', 'National Imagery Transmission Format file']]"},
        {"30314f52444e414e",                                                                                     "[['ntf', 'National Transfer Format Map']]"},
        {"4d52564e",                                                                                             "[['nvram', 'VMware BIOS state file']]"},
        {"4c01",                                                                                                 "[['obj', 'MS COFF relocatable object code']]"},
        {"80",                                                                                                   "[['obj', 'Relocatable object code']]"},
        {"4f67675300020000",                                                                                     "[['oga', 'Ogg Vorbis Codec compressed file'], ['ogg', 'Ogg Vorbis Codec compressed file'], ['ogv', 'Ogg Vorbis Codec compressed file'], ['ogx', 'Ogg Vorbis Codec compressed file']]"},
        {"e4525c7b8cd8a74d",                                                                                     "[['one', 'MS OneNote note']]"},
        {"fdffffff20",                                                                                           "[['opt', 'Developer Studio subheader']]"},
        {"414f4c564d313030",                                                                                     "[['org', 'AOL personal file cabinet'], ['pfc', 'AOL personal file cabinet']]"},
        {"64000000",                                                                                             "[['p10', 'Intel PROset|Wireless Profile']]"},
        {"1a0b",                                                                                                 "[['pak', 'PAK Compressed archive file']]"},
        {"5041434b",                                                                                             "[['pak', 'Quake archive file']]"},
        {"47504154",                                                                                             "[['pat', 'GIMP pattern file']]"},
        {"504158",                                                                                               "[['pax', 'PAX password protected bitmap']]"},
        {"564350434830",                                                                                         "[['pch', 'Visual C PreCompiled header']]"},
        {"0a050101",                                                                                             "[['pcx', 'ZSOFT Paintbrush file_3']]"},
        {"0a030101",                                                                                             "[['pcx', 'ZSOFT Paintbrush file_2']]"},
        {"0a020101",                                                                                             "[['pcx', 'ZSOFT Paintbrush file_1']]"},
        {"4d6963726f736f667420432f432b2b20",                                                                     "[['pdb', 'MS C++ debugging symbols file']]"},
        {"4d2d5720506f636b",                                                                                     "[['pdb', 'Merriam-Webster Pocket Dictionary']]"},
        {"aced000573720012",                                                                                     "[['pdb', 'BGBlitz position database file']]"},
        {"737a657a",                                                                                             "[['pdb', 'PowerBASIC Debugger Symbols']]"},
        {"736d5f",                                                                                               "[['pdb', 'PalmOS SuperMemo']]"},
        {"1100000053434341",                                                                                     "[['pf', 'Windows prefetch file']]"},
        {"504750644d41494e",                                                                                     "[['pgd', 'PGP disk image']]"},
        {"50350a",                                                                                               "[['pgm', 'Portable Graymap Graphic']]"},
        {"9901",                                                                                                 "[['pkr', 'PGP public keyring']]"},
        {"89504e470d0a1a0a",                                                                                     "[['png', 'PNG image']]"},
        {"fdffffff0e000000",                                                                                     "[['ppt', 'PowerPoint presentation subheader_4']]"},
        {"a0461df0",                                                                                             "[['ppt', 'PowerPoint presentation subheader_3']]"},
        {"0f00e803",                                                                                             "[['ppt', 'PowerPoint presentation subheader_2']]"},
        {"006e1ef0",                                                                                             "[['ppt', 'PowerPoint presentation subheader_1']]"},
        {"fdffffff43000000",                                                                                     "[['ppt', 'PowerPoint presentation subheader_6']]"},
        {"fdffffff1c000000",                                                                                     "[['ppt', 'PowerPoint presentation subheader_5']]"},
        {"74424d504b6e5772",                                                                                     "[['prc', 'PathWay Map file']]"},
        {"424f4f4b4d4f4249",                                                                                     "[['prc', 'Palmpilot resource file']]"},
        {"38425053",                                                                                             "[['psd', 'Photoshop image']]"},
        {"7e424b00",                                                                                             "[['psp', 'Corel Paint Shop Pro image']]"},
        {"7b5c707769",                                                                                           "[['pwi', 'MS WinMobile personal note']]"},
        {"e3828596",                                                                                             "[['pwl', 'Win98 password file']]"},
        {"b04d4643",                                                                                             "[['pwl', 'Win95 password file']]"},
        {"458600000600",                                                                                         "[['qbb', 'QuickBooks backup']]"},
        {"ac9ebd8f0000",                                                                                         "[['qdf', 'QDF Quicken data']]"},
        {"51454c20",                                                                                             "[['qel', 'QDL Quicken data']]"},
        {"514649",                                                                                               "[['qemu', 'Qcow Disk Image']]"},
        {"03000000",                                                                                             "[['qph', 'Quicken price history']]"},
        {"00004d4d585052",                                                                                       "[['qxd', 'Quark Express (Motorola)']]"},
        {"00004949585052",                                                                                       "[['qxd', 'Quark Express (Intel)']]"},
        {"2e7261fd00",                                                                                           "[['ra', 'RealAudio streaming media']]"},
        {"2e524d4600000012",                                                                                     "[['ra', 'RealAudio file']]"},
        {"727473703a2f2f",                                                                                       "[['ram', 'RealMedia metafile']]"},
        {"526172211a0700",                                                                                       "[['rar', 'WinRAR compressed archive']]"},
        {"52454745444954",                                                                                       "[['reg', 'WinNT Registry|Registry Undo files'], ['sud', 'WinNT Registry|Registry Undo files']]"},
        {"fffe",                                                                                                 "[['reg', 'Windows Registry file']]"},
        {"01da01010003",                                                                                         "[['rgb', 'Silicon Graphics RGB Bitmap']]"},
        {"2e524d46",                                                                                             "[['rm', 'RealMedia streaming media'], ['rmvb', 'RealMedia streaming media']]"},
        {"edabeedb",                                                                                             "[['rpm', 'RedHat Package Manager']]"},
        {"43232b44a4434da5",                                                                                     "[['rtd', 'RagTime document']]"},
        {"7b5c72746631",                                                                                         "[['rtf', 'RTF file']]"},
        {"5b5645525d",                                                                                           "[['sam', 'Lotus AMI Pro document_1']]"},
        {"5b7665725d",                                                                                           "[['sam', 'Lotus AMI Pro document_2']]"},
        {"24464c3240282329",                                                                                     "[['sav', 'SPSS Data file']]"},
        {"534d415254445257",                                                                                     "[['sdr', 'SmartDraw Drawing file']]"},
        {"4848474231",                                                                                           "[['sh3', 'Harvard Graphics presentation file']]"},
        {"67490000",                                                                                             "[['shd', 'Win2000|XP printer spool file']]"},
        {"4b490000",                                                                                             "[['shd', 'Win9x printer spool file']]"},
        {"66490000",                                                                                             "[['shd', 'WinNT printer spool file']]"},
        {"68490000",                                                                                             "[['shd', 'Win Server 2003 printer spool file']]"},
        {"53484f57",                                                                                             "[['shw', 'Harvard Graphics presentation']]"},
        {"5374756666497420",                                                                                     "[['sit', 'StuffIt compressed archive']]"},
        {"5349542100",                                                                                           "[['sit', 'StuffIt archive']]"},
        {"07534b46",                                                                                             "[['skf', 'SkinCrafter skin']]"},
        {"9501",                                                                                                 "[['skr', 'PGP secret keyring_2']]"},
        {"9500",                                                                                                 "[['skr', 'PGP secret keyring_1']]"},
        {"3a56455253494f4e",                                                                                     "[['sle', 'Surfplan kite project file']]"},
        {"414376",                                                                                               "[['sle', 'Steganos virtual secure drive']]"},
        {"4d6963726f736f66742056697375616c",                                                                     "[['sln', 'Visual Studio .NET file']]"},
        {"001e849000000000",                                                                                     "[['snm', 'Netscape Communicator (v4) mail folder']]"},
        {"fdffffff04",                                                                                           "[['suo', 'Visual Studio Solution subheader']]"},
        {"465753",                                                                                               "[['swf', 'Shockwave Flash player']]"},
        {"435753",                                                                                               "[['swf', 'Shockwave Flash file']]"},
        {"ffffffff",                                                                                             "[['sys', 'DOS system driver']]"},
        {"ff",                                                                                                   "[['sys', 'Windows executable']]"},
        {"ff4b455942202020",                                                                                     "[['sys', 'Keyboard driver file']]"},
        {"414d594f",                                                                                             "[['syw', 'Harvard Graphics symbol graphic']]"},
        {"7573746172",                                                                                           "[['tar', 'Tape Archive']]"},
        {"1fa0",                                                                                                 "[['tar.z', 'Compressed tape archive_2']]"},
        {"1f9d90",                                                                                               "[['tar.z', 'Compressed tape archive_1']]"},
        {"b46e6844",                                                                                             "[['tib', 'Acronis True Image']]"},
        {"4d4d002b",                                                                                             "[['tif', 'TIFF file_4'], ['tiff', 'TIFF file_4']]"},
        {"4d4d002a",                                                                                             "[['tif', 'TIFF file_3'], ['tiff', 'TIFF file_3']]"},
        {"49492a00",                                                                                             "[['tif', 'TIFF file_2'], ['tiff', 'TIFF file_2']]"},
        {"492049",                                                                                               "[['tif', 'TIFF file_1'], ['tiff', 'TIFF file_1']]"},
        {"4d53465402000100",                                                                                     "[['tlb', 'OLE|SPSS|Visual C++ library file']]"},
        {"0110",                                                                                                 "[['tr1', 'Novell LANalyzer capture file']]"},
        {"55434558",                                                                                             "[['uce', 'Unicode extensions']]"},
        {"554641c6d2c1",                                                                                         "[['ufa', 'UFA compressed archive']]"},
        {"454e545259564344",                                                                                     "[['vcd', 'VideoVCD|VCDImager file']]"},
        {"424547494e3a5643",                                                                                     "[['vcf', 'vCard']]"},
        {"5b4d535643",                                                                                           "[['vcw', 'Visual C++ Workbench Info File']]"},
        {"636f6e6563746978",                                                                                     "[['vhd', 'Virtual PC HD image']]"},
        {"4b444d",                                                                                               "[['vmdk', 'VMware 4 Virtual Disk']]"},
        {"23204469736b2044",                                                                                     "[['vmdk', 'VMware 4 Virtual Disk description']]"},
        {"434f5744",                                                                                             "[['vmdk', 'VMware 3 Virtual Disk']]"},
        {"813284c18505d011",                                                                                     "[['wab', 'Outlook Express address book (Win95)']]"},
        {"9ccbcb8d1375d211",                                                                                     "[['wab', 'Outlook address file']]"},
        {"3e000300feff090006",                                                                                   "[['wb3', 'Quatro Pro for Windows 7.0']]"},
        {"0000020006040600",                                                                                     "[['wk1', 'Lotus 1-2-3 (v1)']]"},
        {"00001a0000100400",                                                                                     "[['wk3', 'Lotus 1-2-3 (v3)']]"},
        {"00001a0002100400",                                                                                     "[['wk4', 'Lotus 1-2-3 (v4|v5)'], ['wk5', 'Lotus 1-2-3 (v4|v5)']]"},
        {"ff00020004040554",                                                                                     "[['wks', 'Works for Windows spreadsheet']]"},
        {"0e574b53",                                                                                             "[['wks', 'DeskMate Worksheet']]"},
        {"d7cdc69a",                                                                                             "[['wmf', 'Windows graphics metafile']]"},
        {"ff575043",                                                                                             "[['wp', 'WordPerfect text and graphics'], ['wp5', 'WordPerfect text and graphics'], ['wp6', 'WordPerfect text and graphics'], ['wpd', 'WordPerfect text and graphics'], ['wpg', 'WordPerfect text and graphics'], ['wpp', 'WordPerfect text and graphics']]"},
        {"81cdab",                                                                                               "[['wpf', 'WordPerfect text']]"},
        {"4d6963726f736f66742057696e646f7773204d6564696120506c61796572202d2d20",                                 "[['wpl', 'Windows Media Player playlist']]"},
        {"32be",                                                                                                 "[['wri', 'MS Write file_2']]"},
        {"31be",                                                                                                 "[['wri', 'MS Write file_1']]"},
        {"be000000ab",                                                                                           "[['wri', 'MS Write file_3']]"},
        {"1d7d",                                                                                                 "[['ws', 'WordStar Version 5.0|6.0 document']]"},
        {"575332303030",                                                                                         "[['ws2', 'WordStar for Windows file']]"},
        {"fdffffff29",                                                                                           "[['xls', 'Excel spreadsheet subheader_7']]"},
        {"fdffffff28",                                                                                           "[['xls', 'Excel spreadsheet subheader_6']]"},
        {"fdffffff23",                                                                                           "[['xls', 'Excel spreadsheet subheader_5']]"},
        {"fdffffff22",                                                                                           "[['xls', 'Excel spreadsheet subheader_4']]"},
        {"fdffffff1f",                                                                                           "[['xls', 'Excel spreadsheet subheader_3']]"},
        {"fdffffff10",                                                                                           "[['xls', 'Excel spreadsheet subheader_2']]"},
        {"0908100000060500",                                                                                     "[['xls', 'Excel spreadsheet subheader_1']]"},
        {"3c3f786d6c2076657273696f6e3d22312e30223f3e",                                                           "[['xml', 'User Interface Language']]"},
        {"5850434f4d0a5479",                                                                                     "[['xpt', 'XPCOM libraries']]"},
        {"4d5a90000300000004000000ffff",                                                                         "[['zap', 'ZoneAlam data file']]"},
        {"57696e5a6970",                                                                                         "[['zip', 'WinZip compressed archive']]"},
        {"504b030414000100",                                                                                     "[['zip', 'ZLock Pro encrypted ZIP']]"},
        {"504b0708",                                                                                             "[['zip', 'PKZIP archive_3']]"},
        {"504b0506",                                                                                             "[['zip', 'PKZIP archive_2']]"},
        {"504b537058",                                                                                           "[['zip', 'PKSFX self-extracting archive']]"},
        {"504b4c495445",                                                                                         "[['zip', 'PKLITE archive']]"},
        {"5a4f4f20",                                                                                             "[['zoo', 'ZOO compressed archive']]"},
        {"deadbeef",                                                                                             "['*', 'DEAD BEEF']"},
        {"ffd8ff",                                                                                               "['jpg', 'JPEG ALL']"},
        {"0a0501",                                                                                               "['pcx', 'PCX ALL']"},
        {"0000001c66747970",                                                                                     "['mpeg', 'MPEG-4 Video']"},
        {"0000002466747970",                                                                                     "['mpeg', 'MPEG-4 Video']"},
        {"fffbb0",                                                                                               "['mp3', 'MP3']"},
        {"fffb90",                                                                                               "['mp3', 'MP3']"},
        {"fffb94",                                                                                               "['mp3', 'MP3']"},
        {"fffb54",                                                                                               "['mp3', 'MP3']"},
        {"fffbd4",                                                                                               "['mp3', 'MP3']"}
};


/**
 * The main scan function
 *
 * @param root_path
 * @param verbose
 */
void main_scan(char *root_path, bool verbose) {
    struct timespec start, end;
    clock_gettime(CLOCK_REALTIME, &start);

    // prepare the Signatures
    sort_signatures(g_well_known_mn);

    // creates the CSV file: open it
    srand(time(NULL));
    if (!create_report("./outcome", verbose))
        fprintf(stderr, "CSV file output problem\n");

    // start the scanning
    printf("+ %s", root_path);
    p_scan_files(root_path, 2, verbose);

    // close the file
    close_file();

    clock_gettime(CLOCK_REALTIME, &end);

    // time_spent = end - start
    double time_spent = (end.tv_sec - start.tv_sec) +
                        (end.tv_nsec - start.tv_nsec) / BILLION;

    printf("\n\n\n");
    printf("\n---------------------------- STATS ---------------------------- ");
    printf("\nNumber of files scanned:                                      %d", g_stats.num_files);
    printf("\nNumber of files with High Entropy:                            %d",
           g_stats.num_files_with_high_entropy);
    printf("\nNumber of files with low Entropy:                             %d",
           g_stats.num_files_with_low_entropy);
    printf("\nNumber of files with Well Known Magic Number:                 %d",
           g_stats.num_files_with_well_known_magic_number);
    printf("\nNumber of files with zero size or less of magic number_s size:  %d",
           g_stats.num_files_with_size_zero_or_less);
    printf("\nNumber of files with length < min_size:                       %d", g_stats.num_files_with_min_size);
    printf("\nNumber of files with ERRS:                       %d", g_stats.num_files_with_errs);

    printf("\nTime elpased is %f seconds", time_spent);
    printf("\n---------------------------- ***** ---------------------------- ");
}

/**
 * Read a binary file in memory with the magic number_s (first 4 bytes)
 *
 * @param path
 * @param length
 * @param magic_number_out
 * @return
 */
unsigned char *p_read_file_content(FILE *fp, char *path, long length) {

    unsigned char *buffer = NULL;
    if (fseek(fp, 0, SEEK_SET) == 0) {
        buffer = (unsigned char *) malloc(length * sizeof(unsigned char));
        fread(buffer, sizeof(unsigned char), length, fp);
    }
    return buffer;
}

/**
 * Read the magic number
 *
 * @param fp
 * @return
 */
unsigned char *p_read_magic_number(FILE *fp) {

    unsigned char *buffer_mn = NULL;

    if (fseek(fp, 0, SEEK_SET) == 0) {
        // Allocates the buffer for the magic byte
        buffer_mn = malloc(sizeof(unsigned char) * MAGIC_NUMBER_BYTE_SIZE + 1);
        fread(buffer_mn, sizeof(unsigned char), MAGIC_NUMBER_BYTE_SIZE, fp);
        buffer_mn[MAGIC_NUMBER_BYTE_SIZE] = '\0';
    }

    return buffer_mn;
}

/**
 * Scan recursively the base_path
 *
 * @param base_path
 * @param indent
 * @param verbose
 */
void p_scan_files(char *base_path, int indent, bool verbose) {
    int i;
    char path[MAX_PATH_BUFFER];
    struct dirent *dp;
    DIR *dir = opendir(base_path);

    if (dir == NULL) {
        g_stats.num_files++;
        p_scan_file(base_path, verbose);
        return;
    }

    while ((dp = readdir(dir)) != NULL) {
        if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0) {
            if (verbose) {
                printf("\n  ");
                for (i = 0; i < indent; i++)
                    printf(" ");
            }

            strcpy(path, base_path);
            strcat(path, "/");
            strcat(path, dp->d_name);

            (verbose) ? printf("|- %s ", dp->d_name) : 0;

            p_scan_files(path, indent + 2, verbose);
        }
    }

    closedir(dir);
}

/**
 * Scan a file
 *
 * @param basePath
 * @param verbose
 */
void p_scan_file(char *basePath, bool verbose) {
    long file_length = -1;

    // flags
    bool magic_number_found = false;
    bool has_high_entropy = false;
    bool has_size_zero_or_less = false;
    bool has_min_size = false;
    bool has_errs = false;
    char err_description[MAX_PATH_BUFFER+256]="";
    double H = -1;
    char report_line_buffer[MAX_PATH_BUFFER];

    // take the access, creation and modification file timestamp
    struct stat attr;
    stat(basePath, &attr);
    char *mtime_s = strtok(ctime(&attr.st_mtime), "\n");
    char *ctime_s = strtok(ctime(&attr.st_ctime), "\n");
    char *atime_s = strtok(ctime(&attr.st_atime), "\n");
    file_length = attr.st_size;

    // exit for file too small
    if (file_length < MAGIC_NUMBER_BYTE_SIZE || file_length < MIN_FILE_SIZE) {
        if (file_length < MAGIC_NUMBER_BYTE_SIZE) {
            g_stats.num_files_with_size_zero_or_less++;
            has_size_zero_or_less = true;
        } else if (file_length < MIN_FILE_SIZE) {
            g_stats.num_files_with_min_size++;
            has_min_size = true;
        }

    } else {

        FILE *fp = fopen(basePath, "rb");
        int ferror_flags = -1;
        if (fp && ((ferror_flags = fp->_flag & 0x0020) == 0)) {
        //if (fp) {

            if (file_length > MAX_FILE_SIZE)
                file_length = MAX_FILE_SIZE;

            // read the magic number
            unsigned char *magic_number_string = p_read_magic_number(fp);
            char magic_number_hex_string[MAGIC_NUMBER_BYTE_SIZE * 2 + 1];

            // search if it is a well knwon magic number
            if (magic_number_string != NULL) {
                sprintf(magic_number_hex_string, "%02x%02x%02x%02x", magic_number_string[0], magic_number_string[1],
                        magic_number_string[2],
                        magic_number_string[3]);
                free(magic_number_string);

                //magic_number_found = has_magic_number_simple(magic_number_string);
                magic_number_found = p_binary_search(strtoul(magic_number_hex_string, NULL, 16), 0,
                                                     SIGNATURES_VECTOR_LENGTH - 1);

                if (magic_number_found) {
                    (verbose) ? printf("(magic found: %s)", magic_number_hex_string) : 0;
                    g_stats.num_files_with_well_known_magic_number++;
                } else {

                    unsigned char *content = p_read_file_content(fp, basePath, file_length);
                    if (file_length > MIN_FILE_SIZE && content != NULL) {
                        H = calc_rand_idx(content, file_length);
                        if (H > ENTROPY_TH) {
                            g_stats.num_files_with_high_entropy++;
                            has_high_entropy = true;
                            (verbose) ? printf("(high H: %f)", H) : 0;
                        } else {
                            (verbose) ? printf("(low H: %f)", H) : 0;
                            g_stats.num_files_with_low_entropy++;
                        }

                        free(content);
                    }
                }
            } else {
                // some problem in catching the magic number string
                has_errs = true;
                g_stats.num_files_with_errs++;
                sprintf(err_description, "Magic number string problem in: %s", basePath);
                fprintf(stderr, "\n%s",err_description);

            }
            fclose(fp);
        } else {
            has_errs = true;
            g_stats.num_files_with_errs++;

            if (!fp) {
                sprintf(err_description, "Cannot open the file: %s (file handler is null)", basePath);
                fprintf(stderr, "\n%s",err_description);
            }
            else {
                char buffer[33];
                itoa(ferror_flags, buffer, 2);
                sprintf(err_description, "Cannot open the file: %s (mask bit err: %s)", basePath, buffer);
                fprintf(stderr, "\n%s",err_description);
            }

        }
    }

    (verbose) ? printf(" - l: %ldb", file_length) : 0;

    // Append the line in the CSV file
    sprintf(report_line_buffer, "%s\t%f\t%d\t%d\t%d\t%d\t%d\t%ld\t%s\t%s\t%s\t%s\n",
            basePath,
            H,
            magic_number_found,
            has_errs,
            has_high_entropy,
            has_size_zero_or_less,
            has_min_size,
            file_length,
            ctime_s,
            atime_s,
            mtime_s,
            err_description);
    append_to_report(report_line_buffer);
}


bool has_magic_number_simple(const char *magic_number_string) {
    bool magic_number_found = false;
    for (int j = 0; j < SIGNATURES_VECTOR_LENGTH; j++)
        if (strncmp(g_well_known_mn[j].number8_s, magic_number_string, strlen(magic_number_string)) == 0) {
            magic_number_found = true;
            break;
        }
    return magic_number_found;
}

bool p_binary_search(unsigned long magic_number, int lower, int upper) {
    while (lower <= upper) {
        int mid = (upper + lower) / 2;

        if (g_well_known_mn[mid].number8_ul == magic_number)
            return true;

        if (g_well_known_mn[mid].number8_ul < magic_number)
            lower = mid + 1;
        else
            upper = mid - 1;
    }

    return false;
}

/**
 * Return the file length
 * @param fp
 * @return -1 for problem otherwise the length
 */
long p_read_length(FILE *fp) {

    if (fseek(fp, 0, SEEK_END) == 0)
        return ftell(fp);
    return -1;
}


