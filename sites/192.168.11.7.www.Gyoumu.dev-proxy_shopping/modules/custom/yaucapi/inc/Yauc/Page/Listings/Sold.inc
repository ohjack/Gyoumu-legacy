<?php
namespace Yauc\Page\Listings;

class Sold extends \Gulei\Page\Form\Basic
{
  public function hook_form(&$form_state, $sellerId)
  {
    $this->redirect($form_state);

    $output = '<h1>' . $sellerId . '</h1>';
    $t = new \Yauc\Table\Listings\Sold($sellerId, array('search' => $form_state['storage']['search']));

    $output .= $t->theme(null);

    $form = array();

    $form['searchpanel'] = array(
      '#type' => 'fieldset',
      '#attributes' => array('id' => 'searchpanel'),
    );

    $form['searchpanel']['start_time'] = array(
      '#title' => 'Start Time',
      '#type' => 'textfield',
      '#size' => null,
      '#attributes' => array('widget' => 'datepicker'),
      '#default_value' => $_GET['start_time']
    );

    $form['searchpanel']['end_time'] = array(
      '#title' => 'End Time',
      '#type' => 'textfield',
      '#size' => null,
      '#attributes' => array('widget' => 'datepicker'),
      '#default_value' => $_GET['end_time']
    );

    $form['searchpanel']['search'] = array(
      '#type' => 'submit',
      '#value' => 'Search',
      '#name' => 'search',
      '#prefix' => '<div style="clear:both"></div>',
    );

    $form['searchpanel']['clear'] = array(
      '#type' => 'submit',
      '#value' => 'Clear',
      '#name' => 'clear',
    );

    $form['list'] = array(
      '#type' => 'markup',
      '#value' => $output,
      '#obj' => $t->save(),
    );

    $form['download'] = array(
      '#type' => 'submit',
      '#value' => 'Download',
      '#name' => 'download',
    );

    jquery_ui_add('ui.datepicker');
    drupal_add_css(drupal_get_path('module', 'yaucapi') . '/yaucapi.css');
    drupal_add_js(drupal_get_path('module', 'yaucapi') . '/yaucapi.js');
    return $form;
  }

  protected function proc_download($form, &$form_state)
  {
    $funcSortDownload = create_function(
      '$a,$b',
      'return strcmp($a["time_end"], $b["time_end"]);'
    );

    $t = $form['list']['#obj']->renew();

    $form_state['storage']['redirect'] =
      $t->getCsvFile(
        array('sort' => $funcSortDownload),
        'YahooOrderRaw'
      );
  }

  protected function proc_searchpanel_search($form, &$form_state)
  {
    $this->goBack(false);
  }

  protected function proc_searchpanel_clear()
  {
    $this->goBack();
  }
}