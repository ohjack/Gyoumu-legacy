<?php
namespace Ebapi\Form\Widget\Fitment;

class Found extends _Abstract
{
  public static function getEntryName()
  {
    return 'fitment';
  }

  private function isShowFitment()
  {
    return $this->getValue()
           && $this->getValue(\Ebapi\Form\Widget\Fitment\Select\Make::getEntryName())
           && $this->getValue(\Ebapi\Form\Widget\Fitment\Select\Model::getEntryName())
        ;
  }

  public function getFormEntry()
  {
    $form = array();
    $form['add-fitment'] = $this->getEleAhah(
      array(
           '#weight' => 2,
           '#type' => 'submit',
           '#value' => 'Add',
      )
    );

    if($this->isShowFitment()){
      $form['list'] = array(
        '#weight' => 1,
        '#type' => 'checkboxes',
        '#title' => 'Fitment <span class="toggle-all">toggle all</span>',
        '#prefix' => '<div id="ebapi-fitment-list">',
        '#suffix' => '</div>',
      );
    }
    else{
      $form['msg'] = array(
        '#type' => 'markup',
        '#value' => '<div><em>Select at least Make and Model.</em></div>',
      );

      $form['add-fitment']['#attributes']['style'] = 'display:none;';
      return $form;
    }

    $pars = array_keys($this->getValue());

    $q = new \Ebapi\Query\Fitment(
      $this->getCri($pars), $this->getYearRange());

    $recs = $q->getFitmentList();

    if(empty($recs)){
      $form['list']['#suffix'] = '<div><em>No fitment found.</em></div>';
      $form['add-fitment']['#attributes']['style'] = 'display:none;';
    }
    else{
      $sels = $this->getSelectedValue();

      foreach($recs as $ePid => $name){
        $form['list'][$ePid] = array(
          '#type' => 'checkbox',
          '#title' => $name,
          '#return_value' => $ePid,
        );

        if(isset($sels[$ePid])){
          $form['list'][$ePid]['#attributes'] = array(
            'checked' => 'checked',
            'disabled' => 'disabled',
          );
        }
      }
    }
    return $form;
  }
}