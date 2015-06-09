<?php
namespace Ebapi\Compatibility;

class _List extends \Ebapi\Form\Widget\Fitment\_Abstract
{
  public static function getEntryName()
  {
    return '';
  }

  private function getSelectorEles()
  {
    $form = array();

    $par_set = array(
      'make' => array(\Ebapi\Form\Widget\Fitment\Select\Make::getEntryName()),
      'make_model' => array(\Ebapi\Form\Widget\Fitment\Select\Make::getEntryName(), \Ebapi\Form\Widget\Fitment\Select\Model::getEntryName()),
    );

    $wids = array(
      new \Ebapi\Form\Widget\Fitment\Select\Make($this->form_state, array()),
      new \Ebapi\Form\Widget\Fitment\Year($this->form_state),
      new \Ebapi\Form\Widget\Fitment\Select\Model($this->form_state, $par_set['make']),
      new \Ebapi\Form\Widget\Fitment\Select\Submodel($this->form_state, $par_set['make_model']),
      new \Ebapi\Form\Widget\Fitment\Select\Body($this->form_state, $par_set['make_model']),
      new \Ebapi\Form\Widget\Fitment\Found($this->form_state),
    );

    foreach($wids as $wid){
      $form[$wid->getEntryName()] = $wid->getFormEntry();
    }

    $form['#prefix'] = '<div id="ebapi-fitment-selector">';
    $form['#suffix'] = '</div>';
    return $form;
  }

  private function getListEles()
  {
    $form = array();

    $form['selected'] = array(
      '#type' => 'hidden',
    );

    $form['list'] = array(
      '#title' => 'Compatibility List <span class="toggle-all">toggle all</span>',
      '#type' => 'checkboxes',
      '#prefix' => '<hr/><div id="ebapi-compatibility-list">',
      '#suffix' => '</div>',
    );

    $form['del-fitment'] = $this->getEleAhah(
      array(
           '#type' => 'submit',
           '#value' => 'Remove',
      )
    );

    $dval = $this->getSelectedValue();

    if(empty($dval)){
      $form['selected']['#default_value'] = '{}';
      $form['list']['#options'] = array();
      $form['del-fitment']['#attributes']['style'] = 'display: none;';
      $form['msg-empty'] = array(
        '#type' => 'markup',
        '#value' => '<div><em class="list-empty">List is empty. Click on the ItemID to import from online listings.</em></div>',
      );
    }
    else{
      $form['update-fitment'] = array(
        '#type' => 'submit',
        '#value' => 'Update',
        '#prefix' => '<hr/>',
//        '#suffix' => '<div><em>All fitments in Compatibility List will be submitted. No need to check them on.</em></div>',
      );

      $form['selected']['#default_value'] = json_encode($dval);
      $form['list']['#options'] = $dval;
    }
    return $form;
  }

  public function getFormEntry()
  {
    $form = array();
    $form['fitment-selector'] = $this->getSelectorEles();
    $form['list'] = $this->getListEles();

    drupal_add_css(drupal_get_path('module', 'ebapi_fitment') . '/ebapi_fitment.css');
    drupal_add_js(drupal_get_path('module', 'ebapi_fitment') . '/ebapi_fitment.js');
    return $form;
  }
}