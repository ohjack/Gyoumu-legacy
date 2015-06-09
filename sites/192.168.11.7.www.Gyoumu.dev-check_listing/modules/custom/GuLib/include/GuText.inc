<?php

module_load_include('inc', 'GuLib', '/lib/JapaneseUtil');
module_load_include('php', 'GuLib' , '/lib/Inflector');

class GuTextHelper
{
  static public function asciiize($str){
    $i = new Doctrine_Inflector();
    return $i->unaccent($str);    
  }
  
  static public function kana2roma($str){
    $c = new kana2roma();
    return $c->conv($str);
  }
  
  static public function isAllKana($str){
    $str = mb_convert_kana($str);
    $hirakana = 'ぁあぃいぅうぇえぉおかがきぎくぐけげこごさざしじすずせぜそぞただちぢっつづてでとどなにぬねのはばぱひびぴふぶぷへべぺほぼぽまみむめもゃやゅゆょよらりるれろゎわゐゑをん';
    $katakana = 'ァアィイゥウェエォオカガキギクグケゲコゴサザシジスズセゼソゾタダチヂッツヅテデトドナニヌネノハバパヒビピフブプヘベペホボポマミムメモャヤュユョヨラリルレロヮワヰヱヲン';
    $allKana = $hirakana . $katakana;
    
    return !preg_match('/[^' . $allKana . ']/', $str);
  }
  
  function normalizeNum($str){
    $str = preg_replace('/[- ー－　（）]/u', '', $str);
    $str = mb_strtr($str,
      array(
        '０' => '0',
        '１' => '1',
        '２' => '2',
        '３' => '3',
        '４' => '4',
        '５' => '5',
        '６' => '6',
        '７' => '7',
        '８' => '8',
        '９' => '9',
      ), 'utf8'
    );

    return $str;
  }

  static public function compressHtml($src){
    $src = str_replace(array("\r\n", "\r"), "\n", $src);
    $lines = explode("\n", $src);
    $new_lines = array();

    foreach ($lines as $line) {
        if(!empty($line))
            $new_lines[] = trim($line);
    }
    return implode($new_lines);
  }
}

?>