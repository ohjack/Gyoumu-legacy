<?php
namespace Oerp\Util;

class _Array extends \Gulei\_Array\Basic
{
  public function inherit(&$recs, $prefix = null, &$anchor = null){
    if(empty($recs)){
      $recs[] = array();
      $anchor = &$recs[0];
    }
    $rec = array();

    foreach($this as $name => $each){
      if(is_numeric($name)){
        $inner = new \Oerp\Util\_Array($each);

        if((int)$name > 0){
          $recs[] = array();
          $last_ind = count($recs) - 1;
          $recs[$last_ind] = $inner->inherit($recs, $prefix, $recs[$last_ind]);
        }
        else{
          $inner_rec = $inner->inherit($recs, $prefix, $anchor);
          $rec = array_merge($anchor, $inner_rec);

          if(empty($recs[0])){
            $anchor = array_merge($anchor, $rec);
          }
        }
      }
      else{
        $key = empty($prefix) ? $name : $prefix . '.' . $name;

        if(is_array($each)){
          $inner = new \Oerp\Util\_Array($each);
          $rec = array_merge($rec, $inner->inherit($recs, $key, $rec));
        }
        else{
          $rec[$key] = $each;
        }
      }
    }

    return $rec;
  }

  public function supplement($fld_names, $method){
    $recs = array();
    $this->inherit($recs);

    if(empty($recs[0])){
      return array();
    }

    $name_chks = array();
    foreach($fld_names as $fld_name){
      $rel = explode('.', $fld_name);
      array_pop($rel);

      if($rel){
        $name_chks[$fld_name] = $rel;
      }
    }

    foreach ($recs as $ind => &$rec) {
      $isRoot = false;

      $rec_name_chks = array();
      foreach(array_keys($rec) as $name){
        $rel = explode('.', $name);
        array_pop($rel);

        if($rel){
          $rec_name_chks[] = $rel;
        }
        else{
          $isRoot = true;
          break;
        }
      }

      if($isRoot){
        continue;
      }

      foreach ($fld_names as $fld_name) {
        if(!isset($rec[$fld_name])){
          $isDitto = true;
          foreach($rec_name_chks as $rec_name_chk){
            if(!$chk = $name_chks[$fld_name]){
              break;
            }

            if(!array_diff_assoc($rec_name_chk, $chk)){
              $isDitto = false;
              break;
            }
          }

          if($isDitto){
            switch ($method) {
              case 'copy':
              case 'duplicate':
                for($i = $ind - 1; $ind > 0; $ind--){
                  if(isset($recs[$i][$fld_name])){
                    $rec[$fld_name] = $recs[$i][$fld_name];
                    break;
                  }
                }
                break;

              case 'merge':
              case 'ditto':
                $rec[$fld_name] = '_DITTO_';
                break;
            }
          }
        }
      }
    }

    return $recs;
  }
}