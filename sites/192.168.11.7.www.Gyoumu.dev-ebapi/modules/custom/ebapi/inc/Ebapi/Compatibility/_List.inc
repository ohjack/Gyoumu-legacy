<?php
namespace Ebapi\Compatibility;

class _List extends \Ebapi\Form\Widget\Fitment\_Abstract
{
  protected function getQuery() {
    if ($this->isSiteUk()) {
      return new \Ebapi\Query\UK\Fitment(
        $this->getCri($this->getPreq()),
        $this->isInYearRange() ? $this->getYearRange() : null
      );
    }
    else {
      return new \Ebapi\Query\Motors\Fitment(
        $this->getCri($this->getPreq()),
        $this->isInYearRange() ? $this->getYearRange() : null
      );
    }
  }

  public static function getEntryName()
  {
    return '';
  }

  protected function isSiteUk() {
    $chk = $_GET['site'] == 'UK'
        || $_POST['site'] == 'UK'
        || $this->form_state['values']['site'] == 'UK';
    return $chk;
  }

  protected function getSite() {
    if (!$site = $_GET['site']) {
      if (!$site = $_POST['site']) {
        if (!$site = $this->form_state['values']['site']) {
          return null;
        }
      }
    }
    return $site;
  }

  protected function srcSiteSel() {
    $cur_site = $this->getSite();
    $cur_site = $cur_site ? $cur_site : 'Motors';

    $sites = array(
      'Motors', 'UK',
    );

    $site_sels = array();
    foreach ($sites as $site) {
      if ($site == $cur_site) {
        $site_sels[$site] = '<strong>' . $site . '</strong>';
      }
      else {
        $q = $_GET;
        $q['site'] = $site;

        if ($q['q'] == 'ebapi/fitment/select/js') {
          $q['q'] = 'ebapi/' . $_POST['accnt'] . '/fitment/edit';
        }

        $q = http_build_query($q);
        $q = html_entity_decode($q);
        $q = urldecode($q);
        $site_sels[$site] = sprintf('<a href="?%s">%s</a>', $q, $site);
      }
    }

    $src = '';
    $src .= '<div class="form-item"><label>Sites:</label>';
    $src .= implode('&nbsp;', $site_sels);
    $src .= '</div>';
    return $src;
  }

  private function getSelectorEles()
  {
    $form = array();

    $form['site-sel'] = array(
      '#type' => 'markup',
      '#value' => $this->srcSiteSel(),
    );

    if ($this->form_state['list_only']) {
      if ($this->isSiteUk()) {
        $wid = new \Ebapi\Form\Widget\Fitment\UK\Found($this->form_state);
      }
      else {
        $wid = new \Ebapi\Form\Widget\Fitment\Motors\Found($this->form_state);
      }
      return $wid->getFormEntry();
    }

    if ($this->isSiteUk()) {
      $par_set = array(
        'make' => array(
          \Ebapi\Form\Widget\Fitment\UK\Select\Make::getEntryName()
        ),
        'make_model' => array(
          \Ebapi\Form\Widget\Fitment\UK\Select\Make::getEntryName(),
          \Ebapi\Form\Widget\Fitment\UK\Select\Model::getEntryName()
        ),
      );

      $wids = array(
        new \Ebapi\Form\Widget\Fitment\UK\Select\Make($this->form_state, array()),
        new \Ebapi\Form\Widget\Fitment\UK\Year($this->form_state),
        new \Ebapi\Form\Widget\Fitment\UK\Select\Model($this->form_state, $par_set['make']),
        new \Ebapi\Form\Widget\Fitment\UK\Select\BodyStyle($this->form_state, $par_set['make_model']),
        new \Ebapi\Form\Widget\Fitment\UK\Select\Variant($this->form_state, $par_set['make_model']),
        new \Ebapi\Form\Widget\Fitment\UK\Found($this->form_state),
      );
    }
    else {
      $par_set = array(
        'make' => array(
          \Ebapi\Form\Widget\Fitment\Motors\Select\Make::getEntryName()
        ),
        'make_model' => array(
          \Ebapi\Form\Widget\Fitment\Motors\Select\Make::getEntryName(),
          \Ebapi\Form\Widget\Fitment\Motors\Select\Model::getEntryName()
        ),
      );

      $wids = array(
        new \Ebapi\Form\Widget\Fitment\Motors\Select\Make($this->form_state, array()),
        new \Ebapi\Form\Widget\Fitment\Motors\Year($this->form_state),
        new \Ebapi\Form\Widget\Fitment\Motors\Select\Model($this->form_state, $par_set['make']),
        new \Ebapi\Form\Widget\Fitment\Motors\Select\Submodel($this->form_state, $par_set['make_model']),
        new \Ebapi\Form\Widget\Fitment\Motors\Select\Body($this->form_state, $par_set['make_model']),
        new \Ebapi\Form\Widget\Fitment\Motors\Found($this->form_state),
      );
    }

    foreach($wids as $wid){
      $form[$wid->getEntryName()] = $wid->getFormEntry();
    }

    $form['site'] = array(
      '#type' => 'hidden',
      '#value' => $_POST['site'] ? $_POST['site'] : $_GET['site'],
    );

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

    if ($this->form_state['list_only']) {
      return $form;
    }

    $form['list'] = $this->getListEles();
    drupal_add_css(drupal_get_path('module', 'ebapi_fitment') . '/ebapi_fitment.css');
    drupal_add_js(drupal_get_path('module', 'ebapi_fitment') . '/ebapi_fitment.js');
    return $form;
  }
}