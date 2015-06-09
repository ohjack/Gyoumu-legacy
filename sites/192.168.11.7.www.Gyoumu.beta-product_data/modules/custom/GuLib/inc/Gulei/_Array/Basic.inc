<?php
namespace Gulei\_Array;

class Basic extends \ArrayObject
{
  private function fixClassname($name)
  {
    if(strpos($name, "\\") === false){
      return __NAMESPACE__ . "\\Command\\" . $name;
    }
    return $name;
  }

  public function exe($name, $args = null)
  {
    $name = $this->fixClassname($name);
    $cmd = new $name($this);
    return $cmd->execute($args);
  }

  public function __call($name, $args)
  {
    return $this->exe($this->fixClassname($name), $args);
  }

  public function promote($key, $DROPKEY = false){
    $newArry = array();

    foreach($this as $item){
      if(!isset($item[$key])){
        dpr($item);
        echo '<pre>';
        throw new \Exception('\Gulei\_Array\Basic: Failed to promote. Key "' . $key . '" doesn\'t exist');
      }

      $ind = (is_array($item[$key])) ? $item[$key][0] : $item[$key];

      if(is_array($item) && $DROPKEY){
        unset($item[$key]);
        if(count($item) == 1){
          $item = array_shift($item);
        }
      }
      $newArry[$ind] = $item;
    }
    return new \Gulei\_Array\Basic($newArry);
  }

  public function bunch($key, $DROPKEY = false, $inner_key = 0){
    $newArry = array();
    foreach($this as $item){
      if(!empty($item[$key])){
        $ind = (is_array($item[$key])) ? $item[$key][$inner_key] : $item[$key];

        if($DROPKEY){
          unset($item[$key]);
        }

        if(!isset($newArray[$ind])){
          $newArray[$ind] = array();
        }
        $newArry[$ind][] = $item;
      }
    }
    return $newArry;
  }

  public function bunchKey($key, $inner_key = 0){
    return array_keys($this->bunch($key, false, $inner_key));
  }

  public function pick($key){
    $newArry = new \Gulei\_Array\Basic();
    foreach($this as $item){
      $newArry[] = $item[$key];
    }
    return $newArry;
  }

  public function link($name, $ref){
    foreach($this as &$item){
      if(isset($item[$name])){
        $ind = (is_array($item[$name])) ? $item[$name][0] : $item[$name];
        $item[$name] = $ref[$ind][0];
      }
    }
    return $this;
  }

  public function linkMany($name, $ref){
    foreach($this as &$item){
      if(is_array($item[$name])){
        foreach($item[$name] as &$mem){
          $mem = $ref[$mem][0];
        }
      }else{
        $item[$name] = $ref[$item[$name]][0];
      }
    }
    return $this;
  }

  public function filter($fn, $offset = 0, $result_limit = -1, $scan_limit = -1){
    if((array)$this === array()){
      return new \Gulei\_Array\Basic();
    }

    $it = new \LimitIterator($this->getIterator(), $offset, $scan_limit);
    $fn = create_function('$ary,$key,$item', $fn);
    $result = new \Gulei\_Array\Basic();

    foreach($it as $val){
      if($fn($this, $it->key(), $val)){
        $result[] = $val;

        if($result_limit > -1 && count($result) >= $result_limit){
          break;
        }
      }
    }
    return $result;
  }

  public function walk($fn, $offset = 0, $result_limit = -1, $scan_limit = -1){
    $it = new \LimitIterator($this->getIterator(), $offset, $scan_limit);
    $fn = create_function('$ary,$key,$item', $fn);
    $count = 0;

    foreach($it as $val){
      $fn(&$this, $it->key(), $val);
      $count++;

      if($result_limit > -1 && $count >= $result_limit){
        break;
      }
    }
    return $this;
  }

  public function bunchNumSort($key, $COUNT = false){
    $vals = $this->bunch($key);

    uasort($vals, create_function(
        '$a, $b', 'return count($b) - count($a);'));

    if($COUNT){
      foreach($vals as &$val){
        $val = count($val);
      }
    }
    return $vals;
  }

  public function sort($key){
    $vals = (array)$this;

    uasort($vals, create_function(
      '$a, $b', 'return count($b) - count($a);'));

    return $vals;
  }

  public function usort($callback){
    $arry = (array)$this;
    usort($arry, create_function('$a,$b', $callback));
    $this->exchangeArray($arry);
  }

  public function testWhile($callback){
    $prev = $this[0];
    foreach($this as $item){
      if(call_user_func($callback, $item) === false){
        return $prev;
      }else{
        $prev = $item;
      }
    }
    return $prev;
  }

  public function testUntil($callback){
    $prev = $this[0];
    foreach($this as $item){
      if(call_user_func($callback, $item) === true){
        return $item;
      }else{
        $prev = $item;
      }
    }
    return $prev;
  }

  public function dumpCsv($encoding = null){
    $recs = (array)$this;
    $hds = $recs[0];

    foreach($recs as $ind => &$rec){
      foreach($rec as &$cell){
        if(is_array($cell)){
          throw new \Exception();
        }
        if(preg_match('/[,"\n]/', $cell)){
          $cell = '"' . str_replace('"', '""', $cell) . '"';
        }
      }

      if($ind > 0) {
        $rec_send = array();
        foreach ($hds as $hd) {
          $rec_send[] = $rec[$hd];
        }
      }
      else{
        $rec_send = $rec;
      }

      $rec = implode(',', $rec_send);
    }
    $recs = implode("\r\n", $recs);

    if($encoding){
      return iconv('UTF-8', $encoding . '//TRANSLIT', $recs);
    }
    else{
      return $recs;
    }
  }

  public function mapping($mapping)
  {
    $mappedRecs = array();
    $arch = new \Gulei\Arch\Element($mapping);

    foreach ($arch->xp('./field', $arch->getArch()) as $field) {
      $name = $field->getAttribute('name');

      if ($arch->xp('./field[1]', $field)->length) {
        $inner_mapped = $this->mapping($field);

        foreach($this as $ind => $src_rec){
          $mappedRecs[$ind][$name] = $inner_mapped[$ind];
        }
      }
      else {
        foreach ($this as $ind => $src_rec) {
          $from = $field->getAttribute('from');
          $mappedRecs[$ind][$name] = $src_rec[$from];
        }
      }
    }
    return $mappedRecs;
  }
}