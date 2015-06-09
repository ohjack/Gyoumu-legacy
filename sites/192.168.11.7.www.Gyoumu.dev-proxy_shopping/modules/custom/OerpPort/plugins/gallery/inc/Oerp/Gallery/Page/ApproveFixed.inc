<?php
namespace Oerp\Gallery\Page;

class ApproveFixed extends \Gulei\Page\Form\_Abstract {
  const PAGER_LIMIT = 10;

  private $root;
  private $notes;
  private $fixed;
  private $empty;
  protected $cacheAllFixed;
  protected $cacheOwnFixed;

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

  protected function getMgr() {
    return $_GET['mgr'];
  }

  protected function getOwnSkus() {
    $mgr = $this->getMgr();

    if (!$mgr) {
      return FALSE;
    }

    $q = new \Oerp\Query\Basic(
      'product.product', array(array('product_manager', '=', $mgr)), array('default_code'));

    $skus = $q->raw()->collect('default_code');
    return array_fill_keys($skus, TRUE);
  }

  protected function getAllFixed() {
    if (is_null($this->cacheAllFixed)) {
      $ff = new \Gulei\File\Finder\Local($this->root . '/tofix/fixed');
      return $this->cacheAllFixed = $ff->getList('*.jpg');
    }
    else {
      return $this->cacheAllFixed;
    }
  }

  protected function extractSku($path) {
    preg_match('@.*/_?(..-\d{5})_.*.jpg@', $path, $m);
    return $m[1];
  }

  protected function getAllSkus() {
    $all_fixed = $this->getAllFixed();
    $skus = array();

    foreach ($all_fixed as $each_path) {
      if ($sku = $this->extractSku($each_path)) {
        $skus[] = $sku;
      }
    }
    return $skus;
  }

  protected function organizeFixed($fixed, $skus = NULL) {
    $orged = array();
    $toFilter = $skus ? TRUE : FALSE;

    foreach ($fixed as $path) {
      $sku = $this->extractSku($path);
      if ($toFilter && !$skus[$sku]) {
        continue;
      }

      if ($this->isManipulated($path)) {
        $fn = $this->extractFilename($path);
        preg_match('/^_(.*)_\(/', $fn, $m);
        $orged['review'][$m[1] . '.jpg'] = $path;
      }
      else {
        $orged['raw'][$sku][] = $path;
      }
    }
    return $orged;
  }

  protected function getOwnFixed() {
    if (!is_null($this->cacheOwnFixed)) {
      return $this->cacheOwnFixed;
    }

    $skus = $this->getOwnSkus();

    if ($skus === FALSE) {
      return FALSE;
    }

    $all_fixed = $this->getAllFixed();
    $fixed = $this->organizeFixed($all_fixed, $skus);

    return $this->cacheOwnFixed = $fixed;
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
    $img_src = sprintf($img_tpl, $path);

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

  protected function getFilterEle() {
    $E = new \Oerp\Gallery\Form\Element\PMgrFilter();
    return $E->getEle($this->getAllSkus(), true);
  }

  protected function getPagerEle($count) {
    pager_query('SELECT ' . $count, self::PAGER_LIMIT);

    return array(
      '#type' => 'markup',
      '#value' => theme('pager'),
    );
  }

  protected function sliceFixed($fixed) {
    $page = ($page = $_GET['page']) ? $page : 0;
    $offset = $page * self::PAGER_LIMIT;

    if (!$fixed['raw']) {
      $fixed['raw'] = array();
    }
    else {
      $fixed['raw'] = array_slice($fixed['raw'], $offset, self::PAGER_LIMIT);
    }
    return $fixed;
  }

  protected function isEmpty() {
    $all_fixed = $this->getAllFixed();
    return empty($all_fixed);
  }

  protected function getMsgEle($msg) {
    return array(
      "#type" => 'markup',
      '#value' => $msg,
    );
  }

  protected function hook_form_internal()
  {
    $this->init();
    $this->notes = $this->getRejectNote();

    $form = array();
    $form['filter'] = $this->getFilterEle();

    if (!$this->getMgr()) {
      $all_fixed = $this->getAllFixed();
      $fixed = $this->organizeFixed($all_fixed);
      $this->fixed = $this->sliceFixed($fixed);

      $form['pager'] = $this->getPagerEle(count($fixed['raw']));
    }
    else {
      $fixed = $this->getOwnFixed();
      $this->fixed = $this->sliceFixed($fixed);

      $form['pager'] = $this->getPagerEle(count($fixed['raw']));
    }

    if ($this->isEmpty()) {
      $this->empty = TRUE;
      $form['msg'] = $this->getMsgEle('No picture to approve.');
      return $form;
    }

    elseif (!$this->fixed['raw']) {
      $this->empty = TRUE;
      $form['msg'] = $this->getMsgEle('Select manager above.');
      return $form;
    }

    $ind = 0;
    foreach ($this->fixed['raw'] as $pics) {
      foreach ($pics as $pic) {
        $fn = $this->extractFilename($pic);
        $refixed = $this->fixed['review'][$fn];
        $this->setFixedEle($form, $ind, $pic, $refixed);
        $ind++;
      }
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

  public function hook_theme($form) {
    $buf = '';
    $buf .= drupal_render($form['filter']);

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