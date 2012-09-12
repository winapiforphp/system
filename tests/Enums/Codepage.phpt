--TEST--
Win\System\Codepage Enum
--SKIPIF--
<?php
include __DIR__ . '/../skipif.inc';
?>
--FILE--
<?php
use Win\System\Codepage;
use Win\System\Enum;

$page = new Codepage(Codepage::UTF16);
echo $page, PHP_EOL;
var_dump($page instanceof Enum);
var_dump($page);

?>
= DONE =
--EXPECT--
1200
bool(true)
object(Win\System\Codepage)#1 (2) {
  ["__elements"]=>
  array(149) {
    ["UTF16"]=>
    int(1200)
    ["UTF16BE"]=>
    int(1201)
    ["ANSI"]=>
    int(0)
    ["THREAD_ANSI"]=>
    int(3)
    ["OEM"]=>
    int(1)
    ["MAC"]=>
    int(10000)
    ["UTF7"]=>
    int(65000)
    ["UTF8"]=>
    int(65001)
    ["SYMBOL"]=>
    int(42)
    ["IBM037"]=>
    int(31)
    ["IBM437"]=>
    int(437)
    ["IBM500"]=>
    int(500)
    ["ASMO708"]=>
    int(708)
    ["DOS720"]=>
    int(720)
    ["IMB737"]=>
    int(737)
    ["IBM775"]=>
    int(775)
    ["IBM850"]=>
    int(850)
    ["IBM852"]=>
    int(852)
    ["IBM855"]=>
    int(855)
    ["IMB857"]=>
    int(857)
    ["IBM858"]=>
    int(858)
    ["IBM860"]=>
    int(860)
    ["IBM861"]=>
    int(861)
    ["DOS862"]=>
    int(862)
    ["IBM863"]=>
    int(863)
    ["IBM864"]=>
    int(864)
    ["IBM865"]=>
    int(865)
    ["CP866"]=>
    int(866)
    ["IBM869"]=>
    int(869)
    ["IBM870"]=>
    int(870)
    ["WINDOWS_874"]=>
    int(874)
    ["CP875"]=>
    int(875)
    ["SHIFT_JIS"]=>
    int(932)
    ["GB2312"]=>
    int(936)
    ["KS_C_5601_1987"]=>
    int(949)
    ["BIG5"]=>
    int(950)
    ["IBM01047"]=>
    int(1047)
    ["IBM01140"]=>
    int(1140)
    ["IBM01141"]=>
    int(1141)
    ["IBM01142"]=>
    int(1142)
    ["IBM01143"]=>
    int(1143)
    ["IBM01144"]=>
    int(1144)
    ["IBM01145"]=>
    int(1145)
    ["IBM01146"]=>
    int(1146)
    ["IBM01147"]=>
    int(1147)
    ["IBM01148"]=>
    int(1148)
    ["IBM01149"]=>
    int(1149)
    ["WINDOWS_1250"]=>
    int(1250)
    ["WINDOWS_1251"]=>
    int(1251)
    ["WINDOWS_1252"]=>
    int(1252)
    ["WINDOWS_1253"]=>
    int(1253)
    ["WINDOWS_1254"]=>
    int(1254)
    ["WINDOWS_1255"]=>
    int(1255)
    ["WINDOWS_1256"]=>
    int(1256)
    ["WINDOWS_1257"]=>
    int(1257)
    ["WINDOWS_1258"]=>
    int(1258)
    ["JOHAB"]=>
    int(1361)
    ["X_MAC_JAPANESE"]=>
    int(10001)
    ["X_MAC_CHINESETRAD"]=>
    int(10002)
    ["X_MAC_KOREAN"]=>
    int(10003)
    ["X_MAC_ARABIC"]=>
    int(10004)
    ["X_MAC_HEBREW"]=>
    int(10005)
    ["X_MAC_GREEK"]=>
    int(10006)
    ["X_MAC_CYRILLIC"]=>
    int(10007)
    ["X_MAC_CHINESESIMP"]=>
    int(10008)
    ["X_MAC_ROMANIAN"]=>
    int(10010)
    ["X_MAC_UKRAINIAN"]=>
    int(10017)
    ["X_MAC_THAI"]=>
    int(10021)
    ["X_MAC_CE"]=>
    int(10029)
    ["X_MAC_ICELANDIC"]=>
    int(10079)
    ["X_MAC_TURKISH"]=>
    int(10081)
    ["X_MAC_CROATION"]=>
    int(10082)
    ["X_CHINESE_CNS"]=>
    int(20000)
    ["X_CP20001"]=>
    int(20001)
    ["X_CHINESE_ETEN"]=>
    int(20002)
    ["X_CP20003"]=>
    int(20003)
    ["X_CP20004"]=>
    int(20004)
    ["X_CP20005"]=>
    int(20005)
    ["X_IA5"]=>
    int(20115)
    ["X_IA5_GERMAN"]=>
    int(20116)
    ["X_IA5_SWEDISH"]=>
    int(20117)
    ["X_IA5_NORWEGIAN"]=>
    int(20118)
    ["US_ASCII"]=>
    int(20127)
    ["X_CP20261"]=>
    int(20261)
    ["X_CP20269"]=>
    int(20269)
    ["X_EBCDIC_KOREAN"]=>
    int(20833)
    ["IBM_THAI"]=>
    int(20838)
    ["KOI8_R"]=>
    int(20866)
    ["IBM273"]=>
    int(20273)
    ["IBM277"]=>
    int(20277)
    ["IBM278"]=>
    int(20278)
    ["IBM280"]=>
    int(20280)
    ["IBM284"]=>
    int(20284)
    ["IBM285"]=>
    int(20285)
    ["IBM290"]=>
    int(20290)
    ["IBM297"]=>
    int(20297)
    ["IBM420"]=>
    int(20420)
    ["IBM423"]=>
    int(20423)
    ["IBM424"]=>
    int(20424)
    ["IBM871"]=>
    int(20871)
    ["IBM880"]=>
    int(20880)
    ["IBM905"]=>
    int(20905)
    ["IBM924"]=>
    int(20924)
    ["EUC_JP"]=>
    int(51932)
    ["X_CP20936"]=>
    int(20936)
    ["X_CP20949"]=>
    int(20949)
    ["CP1025"]=>
    int(21025)
    ["KOI8_U"]=>
    int(21866)
    ["ISO_8859_1"]=>
    int(28591)
    ["ISO_8859_2"]=>
    int(28592)
    ["ISO_8859_3"]=>
    int(28593)
    ["ISO_8859_4"]=>
    int(28594)
    ["ISO_8859_5"]=>
    int(28595)
    ["ISO_8859_6"]=>
    int(28596)
    ["ISO_8859_7"]=>
    int(28597)
    ["ISO_8859_8"]=>
    int(28598)
    ["ISO_8859_9"]=>
    int(28599)
    ["ISO_8859_13"]=>
    int(28603)
    ["ISO_8859_15"]=>
    int(28605)
    ["X_EUROPA"]=>
    int(29001)
    ["ISO_8859_8_I"]=>
    int(38598)
    ["ISO_2022_JP"]=>
    int(50220)
    ["CSISO2022JP"]=>
    int(50221)
    ["ISO_2022_JP_SO"]=>
    int(50222)
    ["ISO_2022_KR"]=>
    int(50225)
    ["X_CP50227"]=>
    int(50227)
    ["ISO_2022"]=>
    int(50229)
    ["EBCDIC_JP"]=>
    int(50930)
    ["EBCDIC_US"]=>
    int(50931)
    ["EBCDIC_KR"]=>
    int(50933)
    ["EBCDIC_CN_EX"]=>
    int(50935)
    ["EBCDIC_CN"]=>
    int(50936)
    ["EBCDIC_CN_TRA"]=>
    int(50937)
    ["EBCDIC_JP_EX"]=>
    int(50939)
    ["EUC_CN"]=>
    int(51936)
    ["EUC_KR"]=>
    int(51949)
    ["EUC_CN_TRA"]=>
    int(51950)
    ["HZ_GB_2312"]=>
    int(52936)
    ["GB18030"]=>
    int(54936)
    ["X_ISCII_DE"]=>
    int(57002)
    ["X_ISCII_BE"]=>
    int(57003)
    ["X_ISCII_TA"]=>
    int(57004)
    ["X_ISCII_TE"]=>
    int(57005)
    ["X_ISCII_AS"]=>
    int(57006)
    ["X_ISCII_OR"]=>
    int(57007)
    ["X_ISCII_KA"]=>
    int(57008)
    ["X_ISCII_MA"]=>
    int(57009)
    ["X_ISCII_GU"]=>
    int(57010)
    ["X_ISCII_PA"]=>
    int(57011)
  }
  ["__value"]=>
  int(1200)
}
= DONE =