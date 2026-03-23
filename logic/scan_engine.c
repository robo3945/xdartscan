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
#include "../headers/utils.h"

#define MAGIC_NUMBER_BYTE_SIZE 4
#define SCAN_READ_BUF_SIZE (64 * 1024)

bool p_binary_search(unsigned long magic_number, int lower, int upper);

void p_scan_file(const char *fullPath, unsigned long file_size, bool verbose);

void p_scan_files(const char *base_path, int indent, bool verbose);

void append_line_to_report(const char *fullPath, unsigned long file_length, bool magic_number_found,
                           bool has_high_entropy,
                           bool has_size_zero_or_less, bool has_min_size, bool has_errs, const char *err_description,
                           double H, char *report_line_buffer, const char *magic_number_hex_string, const char *mtime_s,
                           const char *ctime_s, const char *atime_s);

/**
 * Represents a collection of well-known magic numbers and their associated file types or formats.
 *
 * The `g_well_known_mn` array serves as a mapping between specific hexadecimal sequences
 * (known as magic numbers) and the corresponding file formats or types that use these sequences
 * as their headers or identifiers.
 *
 * Each entry consists of:
 * - A magic number (in hexadecimal string format).
 * - A detailed description of potential file formats associated with the magic number,
 *   including file extensions and a brief format or usage description.
 *
 * This array is typically used for identifying and categorizing file formats
 * based on their magic numbers during file analysis or format detection processes.
 */
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
 * Performs the main scanning operation starting from a specified root directory.
 * This function initializes required components, creates report files, scans
 * files in the directory tree, collects statistics, and writes results to the
 * report.
 *
 * @param root_path The root directory path where the scan should begin. The path
 *                  must be valid and accessible.
 * @param verbose   A boolean flag indicating whether verbose output should be
 *                  enabled. If true, detailed progress and error information
 *                  will be displayed.
 */
void main_scan(char *root_path, bool verbose) {
    struct timespec start, end;
    clock_gettime(CLOCK_REALTIME, &start);

    // prepare the Signatures
    sort_signatures(g_well_known_mn);

    // creates the CSV file: open it
    srand(time(NULL));
    const int r = rand();
    if (!create_report_file("./report", r, "tsv", verbose))
        fprintf(stderr, "CSV file output problem\n");

    if (!create_report_file("./stats", r, "txt", verbose))
        fprintf(stderr, "CSV file output problem\n");

    // start the scanning
    printf("+ %s", root_path);
    p_scan_files(root_path, 2, verbose);


    clock_gettime(CLOCK_REALTIME, &end);

    // time_spent = end - start
    const double time_spent = end.tv_sec - start.tv_sec + (end.tv_nsec - start.tv_nsec) / BILLION;

    char buffer[MAX_STATS_BUFFER];
    make_stats(root_path, time_spent, buffer);
    printf("\n");
    printf("%s\n", buffer);
    append_to_report_txt(buffer);

    close_file();
}



/**
 * Recursively scans files and directories from a given base path.
 *
 * @param base_path The base directory path from which the scan begins.
 * @param indent The indentation level for verbose output formatting.
 * @param verbose A flag to enable or disable verbose output during the scan.
 */
void p_scan_files(const char *base_path, const int indent, const bool verbose) {
    struct dirent *dp;

    // Normalize path on the stack instead of heap (malloc/free) to avoid allocation
    // overhead in the recursive scan loop — this function is called once per directory
    char normalized_path[MAX_PATH_BUFFER];
    size_t base_len = strlen(base_path);
    if (base_len >= MAX_PATH_BUFFER) return;
    memcpy(normalized_path, base_path, base_len + 1);
    // Strip trailing slash/backslash to ensure consistent path concatenation below
    if (base_len > 0 && (normalized_path[base_len - 1] == '/' || normalized_path[base_len - 1] == '\\')) {
        normalized_path[--base_len] = '\0';
    }

    DIR *dir = opendir(normalized_path);

    if (dir == NULL) {
        struct stat st;
        if (stat(normalized_path, &st) == 0 && S_ISREG(st.st_mode)) {
            g_stats.num_files++;
            p_scan_file(normalized_path, st.st_size, verbose);
        }
        return;
    }

    while ((dp = readdir(dir)) != NULL) {
        const char *name = dp->d_name;

        // Skip "." and ".." entries by checking raw characters — faster than strcmp/strncmp
        // since we avoid function call overhead for every directory entry
        if (name[0] == '.') {
            if (name[1] == '\0' || (name[1] == '.' && name[2] == '\0'))
                continue;
        }

        if (verbose) {
            printf("\n  ");
            for (int i = 0; i < indent; i++)
                printf(" ");
            printf("|- %s ", name);
        }

        // Build child path on the stack with a single snprintf call — replaces the
        // previous strncpy + strcat approach which required two passes over the string
        char path[MAX_PATH_BUFFER];
        int path_len = snprintf(path, MAX_PATH_BUFFER, "%s/%s", normalized_path, name);
        if (path_len < 0 || path_len >= MAX_PATH_BUFFER) continue;

        // Use dirent's d_type field to classify entries without a stat() syscall.
        // On filesystems that support it (ext4, APFS, etc.), this avoids one stat()
        // per entry, which is a significant speedup on directories with many files.
#ifdef _DIRENT_HAVE_D_TYPE
        if (dp->d_type == DT_REG) {
            struct stat st;
            if (stat(path, &st) == 0) {
                g_stats.num_files++;
                p_scan_file(path, st.st_size, verbose);
            }
        } else if (dp->d_type == DT_DIR) {
            p_scan_files(path, indent + 2, verbose);
        } else {
            // DT_UNKNOWN or other: fallback to recursive call
            p_scan_files(path, indent + 2, verbose);
        }
#else
        p_scan_files(path, indent + 2, verbose);
#endif
    }
    closedir(dir);
}

/**
 * Scans a file and performs several operations such as checking the file's magic number,
 * entropy, size, and errors. It also updates statistics and appends a report summary.
 *
 * @param fullPath The full path to the file to be scanned.
 * @param file_size The file size obtained from stat (avoids redundant stat call).
 * @param verbose A flag indicating whether detailed output should be printed during the scan.
 */
void p_scan_file(const char *fullPath, const unsigned long file_size, const bool verbose) {
    // flags
    bool magic_number_found = false;
    bool has_high_entropy = false;
    bool has_size_zero_or_less = false;
    bool has_min_size = false;
    bool has_errs = false;
    char err_description[MAX_PATH_BUFFER + 256] = "";
    double H = -1;
    char report_line_buffer[MAX_PATH_BUFFER];
    char magic_number_hex_string[MAGIC_NUMBER_BYTE_SIZE * 2 + 1];
    magic_number_hex_string[0] = '\0';

    // Bug fix: ctime() returns a pointer to a single static buffer, so consecutive
    // calls overwrite previous results. Using ctime_r() with separate per-timestamp
    // buffers ensures mtime/ctime/atime are each preserved independently.
    struct stat attr;
    stat(fullPath, &attr);
    char mtime_buf[26], ctime_buf[26], atime_buf[26];
    ctime_r(&attr.st_mtime, mtime_buf);
    ctime_r(&attr.st_ctime, ctime_buf);
    ctime_r(&attr.st_atime, atime_buf);
    // Remove trailing newline
    mtime_buf[strcspn(mtime_buf, "\n")] = '\0';
    ctime_buf[strcspn(ctime_buf, "\n")] = '\0';
    atime_buf[strcspn(atime_buf, "\n")] = '\0';

    unsigned long file_length = file_size;

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
        FILE *fp = fopen(fullPath, "rb");
        if (fp) {
            // Single-read optimization: one fread serves both magic number extraction
            // (first 4 bytes) and entropy calculation (full buffer). This halves the
            // number of read syscalls per file compared to reading magic bytes separately.
            // Cap at MAX_FILE_SIZE to avoid loading huge files entirely into memory.
            unsigned long read_size = file_length;
            if (read_size > (unsigned long)MAX_FILE_SIZE)
                read_size = (unsigned long)MAX_FILE_SIZE;

            unsigned char *content = (unsigned char *)malloc(read_size);
            if (content == NULL) {
                has_errs = true;
                g_stats.num_files_with_errs++;
                snprintf(err_description, sizeof(err_description), "Memory allocation failed for: %s", fullPath);
                fprintf(stderr, "\n%s", err_description);
                fclose(fp);
                goto report;
            }

            size_t bytes_read = fread(content, 1, read_size, fp);
            fclose(fp);

            if (bytes_read < MAGIC_NUMBER_BYTE_SIZE) {
                has_errs = true;
                g_stats.num_files_with_errs++;
                snprintf(err_description, sizeof(err_description), "Read error in: %s", fullPath);
                fprintf(stderr, "\n%s", err_description);
                free(content);
                goto report;
            }

            // Extract magic number hex string from the already-read buffer (no second fread)
            sprintf(magic_number_hex_string, "%02x%02x%02x%02x",
                    content[0], content[1], content[2], content[3]);

            // Convert first 4 bytes to unsigned long via bit-shifting — replaces the
            // previous strtoul(hex_string, ..., 16) approach, avoiding string parsing
            // overhead for every single scanned file
            unsigned long magic_ul = ((unsigned long)content[0] << 24) |
                                     ((unsigned long)content[1] << 16) |
                                     ((unsigned long)content[2] << 8) |
                                     (unsigned long)content[3];

            // Search strategy: try the full 4-byte magic number first, then fall back
            // to a 3-byte prefix match. This catches signatures shorter than 4 bytes.
            int cont = 0;
            while (!magic_number_found && cont < 2) {
                unsigned long search_val = magic_ul >> (8 * cont);
                magic_number_found = p_binary_search(search_val, 0, SIGNATURES_VECTOR_LENGTH - 1);

                if (magic_number_found) {
                    if (verbose) printf("(magic found: %s)", magic_number_hex_string);
                    g_stats.num_files_with_well_known_magic_number++;
                }

                magic_number_hex_string[8 - 2 * (++cont)] = 0;
            }

            if (!magic_number_found) {
                if (file_length > MIN_FILE_SIZE && !THROUGHPUT_TEST) {
                    H = calc_rand_idx(content, bytes_read);
                }

                if (H > ENTROPY_TH) {
                    g_stats.num_files_with_high_entropy++;
                    g_stats.num_files_suspect++;
                    has_high_entropy = true;
                    if (verbose) printf("(high H: %f)", H);
                } else {
                    if (verbose) printf("(low H: %f)", H);
                    g_stats.num_files_with_low_entropy++;
                }
            }

            free(content);
        } else {
            has_errs = true;
            g_stats.num_files_with_errs++;
            snprintf(err_description, sizeof(err_description), "Cannot open the file: %s", fullPath);
            fprintf(stderr, "\n%s", err_description);
        }
    }

report:
    if (verbose) printf(" - l: %lu", file_length);
    g_stats.size_files += file_length;

    append_line_to_report(fullPath, file_length, magic_number_found, has_high_entropy, has_size_zero_or_less,
                          has_min_size, has_errs,
                          err_description, H, report_line_buffer, magic_number_hex_string, mtime_buf, ctime_buf, atime_buf);
}

/**
 * Appends a formatted line containing file details and analysis results to a reporting buffer.
 *
 * @param fullPath The full path of the file being analyzed.
 * @param file_length The size of the file in bytes.
 * @param magic_number_found Indicates whether the file contains a recognized magic number.
 * @param has_high_entropy Indicates whether the file has high entropy.
 * @param has_size_zero_or_less Indicates whether the file size is zero or less than an expected threshold.
 * @param has_min_size Indicates whether the file meets the minimum size requirement.
 * @param has_errs Indicates whether any errors were encountered during file processing.
 * @param err_description A description of any errors encountered during processing.
 * @param H The entropy value of the file content.
 * @param report_line_buffer The buffer where the formatted report line will be stored.
 * @param magic_number_hex_string The hexadecimal representation of the file's magic number if found.
 * @param mtime_s The file's last modification time as a string.
 * @param ctime_s The file's creation time as a string.
 * @param atime_s The file's last access time as a string.
 */
void append_line_to_report(const char *fullPath, unsigned long file_length, bool magic_number_found,
                           bool has_high_entropy,
                           bool has_size_zero_or_less, bool has_min_size, bool has_errs, const char *err_description,
                           double H, char *report_line_buffer, const char *magic_number_hex_string, const char *mtime_s,
                           const char *ctime_s, const char *atime_s) {
    // extract the file component
    char file_name[MAX_PATH_BUFFER] = "";
    const char *p_end_of_path = strrchr(fullPath, '/');
    strncpy(file_name, p_end_of_path + 1, MAX_PATH_BUFFER);

    // extract the extension component
    char ext[MAX_EXT_SIZE] = "";
    const char *p_end_of_file = strrchr(fullPath, '.');
    p_end_of_file ? strncpy(ext, p_end_of_file + 1, MAX_EXT_SIZE) : NULL;

    // Flag files with no recognized magic number and high entropy as potentially encrypted
    int suspect = (!magic_number_found && has_high_entropy) ? 1 : 0;

    snprintf(report_line_buffer, MAX_PATH_BUFFER, "%s\t%s\t%s\t%d\t%f\t%d\t%s\t%d\t%d\t%d\t%d\t%ld\t%s\t%s\t%s\t%s\n",
            fullPath,
            file_name,
            ext,
            suspect,
            H,
            magic_number_found,
            magic_number_hex_string,
            has_errs,
            has_high_entropy,
            has_size_zero_or_less,
            has_min_size,
            file_length,
            ctime_s,
            atime_s,
            mtime_s,
            err_description);

    append_to_report_tsv(report_line_buffer);
}

/**
 * Checks if the given magic number string matches any entry in the predefined
 * well-known magic numbers list.
 *
 * @param magic_number_string The magic number string to check.
 * @return true if the magic number string is found in the predefined list, false otherwise.
 */
bool has_magic_number_simple(const char *magic_number_string) {
    bool magic_number_found = false;
    for (int j = 0; j < SIGNATURES_VECTOR_LENGTH; j++)
        if (strncmp(g_well_known_mn[j].number8_s, magic_number_string, strlen(magic_number_string)) == 0) {
            magic_number_found = true;
            break;
        }
    return magic_number_found;
}

/**
 * Performs a binary search to determine if the specified magic number exists
 * within the range defined by lower and upper indices in a well-known list of
 * magic numbers.
 *
 * @param magic_number The target magic number to search for.
 * @param lower The lower bound index of the search range.
 * @param upper The upper bound index of the search range.
 * @return true if the magic number is found within the range, false otherwise.
 */
bool p_binary_search(const unsigned long magic_number, int lower, int upper) {
    while (lower <= upper) {
        // Prevent integer overflow when computing midpoint: (lower + upper) could overflow,
        // but lower + (upper - lower) / 2 is always safe since upper >= lower
        const int mid = lower + ((upper - lower) >> 1);

        // Cache array value in a local variable to avoid repeated memory lookups
        const unsigned long current = g_well_known_mn[mid].number8_ul;

        if (current == magic_number)
            return true;

        // Branchless-style update: avoids branch misprediction penalty by computing
        // both paths and selecting the correct one via ternary operator
        lower = current < magic_number ? mid + 1 : lower;
        upper = current < magic_number ? upper : mid - 1;
    }

    return false;
}
