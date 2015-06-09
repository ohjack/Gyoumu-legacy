<?php
namespace Checkimage\Page;

class Main
{
  private $state;
  private $providers;

  function __construct($state)
  {
    $this->state = $state;
    $this->providers[] = new \Checkimage\Provider\Local($state);
    $this->providers[] = new \Checkimage\Provider\PictureMaster($state);
    $this->providers[] = new \Checkimage\Provider\Rakuten($state);
  }

  private function getValues($name = null)
  {
    if (is_null($name))
      return $this->state['values'];

    return $this->state['values'][$name];
  }

  private function hasValues()
  {
    return isset($this->state['values']);
  }

  private function getPathSelectionEntry()
  {
    $form = array();

    foreach($this->providers as $p){
      $form[$p->getEntryName()] = $p->getFormEntry($this->state);
    }

    return $form;
  }

  private function getOptionEntry()
  {
    $opts = array();
    $opts['paths'] = $this->getPathSelectionEntry();

    $opts['sku_list'] = array(
      '#title' => 'SKU List',
      '#type' => 'textarea',
      '#description' => 'Enter SKUs by lines.',
      '#default_value' => $this->getSkuList(),
    );

    $opts['load'] = array(
      '#title' => 'Load from database',
      '#type' => 'checkbox',
    );

    $opts['check'] = array(
      '#type' => 'submit',
      '#value' => 'Check',
    );

    $form = array(
      '#title' => 'Check Options',
      '#type' => 'fieldset',
      '#collapsible' => true,
      $opts
    );

    return $form;
  }

  private function getSkuList()
  {
    return $this->getValues('sku_list');
  }

  private function getSelectedOption()
  {
    $paths = array();
    foreach($this->providers as $Provider){
      $paths[$Provider->getEntryName()] = $Provider->getSelectedOption();
    }

    return $paths;
  }

  public function getFormEntry()
  {
    $form = array();
    $form['option'] = $this->getOptionEntry();

    if ($this->hasValues()) {
      $tb = new \Checkimage\View\Result(
        $this->providers
      );

      $form['result'] = array(
        '#type' => 'markup',
        '#value' => $tb->theme()
      );
    }
    return $form;
  }
}