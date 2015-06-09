<?php
namespace Ebapi\Form\Widget\Fitment;

abstract class _Abstract extends \Ebapi\Form\Widget\_Abstract
{
  const prefix = 'select-';

  abstract static public function getEntryName();

  protected function getSelectedValue()
  {
    $dval = (array)json_decode($this->getValue('selected'));
    $dval = array_map('trim', $dval);
    asort($dval);
    return $dval;
  }

  protected function isParentSelected($pars)
  {
    foreach($pars as $par){
      if(!$this->getValue($par)){
        return false;
      }
    }
    return true;
  }


  protected function dropPrefix($name)
  {
    if(!is_string($name))
      return false;

    preg_match('/^' . self::prefix .'(.*)/', $name, $m);
    return $m[1];
  }

  protected function hasPrefixPars($pars)
  {
    foreach($pars as $par){
      if(!is_string($par))
        return true;

      if($this->dropPrefix($par))
        return true;
    }
    return false;
  }

  protected function getCri($pars)
  {
    $hasPrefix = $this->hasPrefixPars($pars);

    $cri = array();
    foreach($pars as $par){
      if($hasPrefix){
        if($drop = $this->dropPrefix($par)){
          $cri[$drop] = $this->getValue($par);
        }
        else{
          continue;
        }
      }
      else{
        $cri[$par] = $this->getValue(self::prefix . $par);
      }
    }
    return $cri;
  }

  protected function getEleAhah($param, $clear = true){
    $tpl = array(
      '#ahah' => array(
        'path' => 'ebapi/fitment/select/js',
        'wrapper' => 'ebapi-compatibility-wrapper',
      ),
    );

    if($clear){
      $tpl['#suffix'] = '<div style="clear:both;"></div>';
    }

    $ele = array_merge($tpl, $param);

    if($ele['#type'] == 'select'){
      $ele['#attributes']['class'] .= ' ebapi-fitment-selector';
    }

    return $ele;
  }

  protected function getYearRange()
  {
    return array(
      $this->getValue('year-from'),
      $this->getValue('year-to'),
    );
  }
}