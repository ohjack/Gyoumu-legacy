<?php
namespace Oerp\Gallery\Page;

class ApproveFixed extends \Gulei\Page\Form\_Abstract
{
  private $root;
  private $notes;
  private $fixed;
  private $empty;

  protected function init()
  {
    $this->root = drupal_get_path('module', 'oerp_gallery') . '/gallery';
  }

  protected function getClassname()
  {
    return __CLASS__;
  }

  protected function getParam()
  {
    return array();
  }

  private function _getRejectNote($path)
  {
    $ff = new \Gulei\File\Finder\Local($this->root . $path);

    $notes = array();
    foreach($ff->getList('*.jpg') as $tofix)
    {
      if(preg_match('@/_([^/]*)_\((.*)\).jpg@', $tofix, $m)){
        $notes[$m[1] . '.jpg'] = $m[2];
      }
    }
    return $notes;
  }

  protected function getRejectNote()
  {
    $notes = array();
    $notes['tofix'] = $this->_getRejectNote('/tofix');
    $notes['fixed'] = $this->_getRejectNote('/tofix/fixed');
    return $notes;
  }

  private function isManipulated($path)
  {
    $fn = $this->extractFilename($path);
    return strpos($fn, '_') === 0;
  }

  protected function getFixed()
  {
    $ff = new \Gulei\File\Finder\Local($this->root . '/tofix/fixed');
    $fixed = array();

    foreach($ff->getList('*.jpg') as $path){
      if($this->isManipulated($path)){
        $fn = $this->extractFilename($path);
        preg_match('/^_(.*)_\(/', $fn, $m);
        $fixed['review'][$m[1] . '.jpg'] = $path;
      }
      else{
        $fixed['raw'][] = $path;
      }
    }
    return $fixed;
  }

  private function extractFilename($path)
  {
    $toks = explode('/', $path);
    return array_pop($toks);
  }

  private function setFixedEle(&$form, $ind, $path, $refixed)
  {
    $prefix = 'pic-' . $ind;

    $fn = $this->extractFilename($path);

    $img_tpl = '<img src="%s" class="gallery"/>';
    $img_src = sprintf($img_tpl, $path, $path);

    if($review = $this->fixed['review'][$fn]){
      $img_src = sprintf($img_tpl, $review, $review);
      $form[$prefix]['#path'] = $review;
    }

    $form[$prefix . '-img'] = array(
      '#type' => 'markup',
      '#value' => $img_src,
    );

    $form[$prefix . '-fname'] = array(
      '#type' => 'value',
      '#value' => $fn,
    );

    if(!$note = $this->notes['fixed'][$fn]){
      $note = $this->notes['tofix'][$fn];
    }

    $form[$prefix . '-note'] = array(
      '#title' => 'Reject Note',
      '#type' => 'textfield',
      '#size' => '',
      '#default_value' => $refixed ? '' : $note,
      '#attributes' => array(
        'class' => 'note',
      ),
      '#suffix' => $refixed ? '(' . $note . ')' : '&nbsp;',
    );

    $form[$prefix . '-delete'] = array(
      '#title' => 'Delete',
      '#type' => 'checkbox',
    );

    $form[$prefix] = array(
      '#title' => 'Approve',
      '#type' => 'checkbox',
      '#attributes' => array(
        'class' => 'approve',
      ),
      '#path' => $path,
      '#refixed' => $refixed,
      '#fn' => $fn,
    );

    if($this->notes['tofix'][$fn]){
      $form[$prefix]['#attributes']['disabled'] = 'disabled';
      $form[$prefix . '-note']['#attributes']['disabled'] = 'disabled';
    }
  }

  protected function hook_form_internal()
  {
    $this->init();
    $this->notes = $this->getRejectNote();
    $this->fixed = $this->getFixed();

    if(!is_array($this->fixed['raw']) || !$this->fixed['raw']){
      $this->empty = true;
      return array(
        'msg' => array("#type" => 'markup', '#value' => 'No picture to approve.'),
      );
    }

    $form = array();

    foreach($this->fixed['raw'] as $ind => $pic){
      $fn = $this->extractFilename($pic);
      $refixed = $this->fixed['review'][$fn];
      $this->setFixedEle(&$form, $ind, $pic, $refixed);
    }
    return $form;
  }

  protected function getToolbarBottom()
  {
    if($this->empty){
      return array();
    }
    $calls = array('Submit');
    return $this->getLocalCalleeEle($calls);
  }

  public function hook_theme($form)
  {
    $buf = '';
    $tplSlot = <<<HTML
<div class="gallery-slot %s">
  <div class="gallery-fname">%s</div>
  <div class="gallery-note">%s</div>
  <div>%s</div>
  <div class="hide">
  %s
  %s
  </div>
</div>
HTML;

    $lastSku = '';

    for($c = 0; isset($form['pic-' . $c]); $c++){
      $prefix = 'pic-' . $c;
      $sku = substr($form[$prefix]['#fn'], 0, 8);

      if($sku != $lastSku){
        $buf .= '<div class="clearfix"></div>';
        $buf .= "<h2>$sku</h2>";
        $lastSku = $sku;
      }

      $buf .= sprintf(
        $tplSlot,
        $form[$prefix]['#attributes']['disabled'] ? 'disabled' : '',
        $form[$prefix . '-fname']['#value'],
        drupal_render($form[$prefix . '-note']),
        drupal_render($form[$prefix . '-img']),
        drupal_render($form[$prefix]),
        drupal_render($form[$prefix . '-delete'])
      );
    }

    $buf .= '<div class="clearfix"></div>';
    $buf .= drupal_render($form);

    $root = drupal_get_path('module', 'oerp_gallery');
    drupal_add_css($root. '/oerp_gallery.css');
    drupal_add_js($root. '/oerp_gallery.js');

    return $buf;
  }
}