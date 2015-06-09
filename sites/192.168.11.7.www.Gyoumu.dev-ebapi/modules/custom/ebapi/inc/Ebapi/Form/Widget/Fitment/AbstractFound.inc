<?php
namespace Ebapi\Form\Widget\Fitment;

abstract class AbstractFound extends \Ebapi\Form\Widget\Fitment\_Abstract
{
  abstract protected function isShowFitment();

  public static function getEntryName()
  {
    return 'fitment';
  }

  public function getFormEntry()
  {
    $list_only = $this->form_state['list_only'];
    $form = array();

    if (!$list_only) {
      $form['add-fitment'] = $this->getEleAhah(
        array(
          '#weight' => 2,
          '#type' => 'submit',
          '#value' => 'Add',
        )
      );
    }

    if($this->isShowFitment()){
      $form['list'] = array(
        '#weight' => 1,
        '#type' => 'checkboxes',
        '#title' => 'Fitment <span class="toggle-all">toggle all</span>',
        '#prefix' => '<div id="ebapi-fitment-list">',
        '#suffix' => '</div>',
      );

      if (!$list_only) {
        $form['list']['#prefix'] = '<div id="ebapi-fitment-list-wrapper">' . $form['list']['#prefix'];
        $form['list']['#suffix'] .= '</div>';
      }
    }
    else{
      $form['msg'] = array(
        '#type' => 'markup',
        '#value' => '<div><em>Select at least Make and Model.</em></div>',
      );

      $form['add-fitment']['#attributes']['style'] = 'display:none;';
      return $form;
    }

    $q = $this->getQuery();
    $recs = $q->getFitmentList();
    $recs = $this->postProcFitmentList($recs);

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

  protected function postProcFitmentList($recs) {
    return $recs;
  }
}