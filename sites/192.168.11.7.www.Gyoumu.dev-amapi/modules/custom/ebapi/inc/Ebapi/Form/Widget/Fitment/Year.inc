<?php
namespace Ebapi\Form\Widget\Fitment;

class Year extends _Abstract
{
  public static function getEntryName()
  {
    return self::prefix . 'year';
  }

  public function getFormEntry()
  {
    $form = array();
    $yfrom = $this->getValue('year-from');
    $yto = $this->getValue('year-to');

    $form['year-from'] = $this->getEleAhah(
      array(
           '#title' => 'Year from',
           '#type' => 'select',
           '#options' => array('0' => '-SELECT-'),
           '#default_value' => ($yfrom) ? $yfrom : '0',
      ),
      false
    );
    $form['year-from']['#attributes']['class'] .= ' year';

    $form['year-to'] = $this->getEleAhah(
      array(
           '#title' => '~ to',
           '#type' => 'select',
           '#options' => array('0' => '-SELECT-'),
           '#default_value' => ($yto) ? $yto : '0',
           '#suffix' => '<div style="clear:both;"></div>',
           '#attributes' => array('class' => 'year no-year-dep'),
      ),
      false
    );
    $form['year-to']['#attributes']['class'] .= ' year';

    $pars = array(self::prefix . 'Make');
    $isReady = $this->isParentSelected($pars);

    if($isReady){
      $q = new \Ebapi\Query\Fitment(
        $this->getCri(array('Make', 'Model', 'Submodel')), null);

      $opts = $q->getSelectOptions('Year');
      $optsFrom = $optsTo = $opts;

      if($yfrom){
        foreach($opts as $key => $val){
          if($key && $val < $yfrom){
            unset($optsTo[$key]);
          }
        }
      }

      if($yto){
        foreach($opts as $key => $val){
          if($key && $val > $yto){
            unset($optsFrom[$key]);
          }
        }
      }

      $form['year-from']['#options'] = $optsFrom;
      $form['year-to']['#options'] = $optsTo;
    }
    return $form;
  }
}

