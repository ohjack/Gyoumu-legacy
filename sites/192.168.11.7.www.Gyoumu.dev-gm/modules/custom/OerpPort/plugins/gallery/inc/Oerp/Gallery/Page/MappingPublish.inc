<?php
namespace Oerp\Gallery\Page;

class MappingPublish extends \Gulei\Page\Form\_Abstract
{
  const ELEPREFIX = 'pub_';
  const SLOT_LIMIT = 8;

  private $accnt;
  private $ts;

  public function __construct(&$form_state = null)
  {
    $this->ts = time();
    parent::__construct($form_state);
  }

  protected function getClassname()
  {
    return __CLASS__;
  }

  protected function getParam()
  {
    return array();
  }

  private function setElesSearch(&$form)
  {
    $eles = array(
      'sku' => array(
        '#title' => 'SKUs',
        '#type' => 'textarea',
      ),
      'suffix' => array(
        '#title' => 'Suffixes',
        '#type' => 'textarea',
      ),
    );

    foreach($eles as $name => $ele){
      $form[$name] = $this->createEle($name, $ele['#type'], $ele);
    }

    $form['search'] = $this->getLocalCalleeEle(
      array('Search')
    );
  }

  private function getPublishFiles($skus, $suffixes)
  {
    $flist = array();
    $groups = array_chunk($skus, 30);

    foreach($groups as $group){
      $fns = array();

      foreach($group as $sku){
        foreach($suffixes as $suffix){
          $fns[] = $sku . '-' . $suffix . '.jpg';
        }
      }

      $pat = '{' . implode(',', $fns) . '}';
      $ff = new \Oerp\Gallery\File\Finder\Gallery('publish/' . $this->accnt);
      $flist = array_merge($flist, $ff->search($pat, false, GLOB_BRACE));
    }

    $pubs = array();
    foreach($flist as $fn){
      preg_match('@.*/(.*)@', $fn, $m);
      $pubs[$m[1]] = $fn;
    }
    return $pubs;
  }

  private function getQueuedFiles()
  {
    $pat = '(' . $this->accnt . '-*.jpg';

    $ff = new \Oerp\Gallery\File\Finder\Gallery('queue');
    $flist = $ff->search($pat);

    $queue = array();
    foreach($flist as $fn){
      preg_match('@.*/\(.*\)(.*)@', $fn, $m);
      $queue[$m[1]] = $fn;
    }
    return $queue;
  }

  private function setElesMapping(&$form, $skus, $suffixes)
  {
    $skus = \Gulei\Text\Helper::explode($skus);
    $suffixes = \Gulei\Text\Helper::explode($suffixes);
    $pubs = $this->getPublishFiles($skus, $suffixes);
    $queued = $this->getQueuedFiles();

    $skus = array_chunk($skus, self::SLOT_LIMIT);
    $page = intval($this->form_state['storage']['page']);

    if(is_array($skus[$page])){
      foreach($skus[$page] as $sku){
        foreach($suffixes as $suffix){
          $fn = $sku . '-' . $suffix . '.jpg';
          $en = self::ELEPREFIX . $sku . '-' . $suffix;

          $form[$en] = $this->createEle(
            $en, 'textfield', array(
              '#size' => '',
              '#sku' => $sku,
              '#fn' => $fn,
              '#attributes' => array(
                'mapped' => $pubs[$fn],
                'queued' => $queued[$fn],
              ),
            )
          );

          $form['del-' . $en] = $this->createEle(
            'del-' . $en, 'checkbox', array(
              '#fn' => $fn
            )
          );
        }
      }
    }

    if(!$skus){
      $form['msg'] = array(
        '#type' => 'markup',
        '#value' => '<div>No picture for mapping.</div>'
      );
    }
    else{
      $pages = count($skus);

      if($page > 0){
        $form['page-prev'] = $this->getLocalCalleeEle(array('Previous_page'));
      }

      if($page + 1 < $pages){
        $form['page-next'] = $this->getLocalCalleeEle(array('Next_page'));
      }

      $form['submit'] = $this->getLocalCalleeEle(array('Submit'));
    }
  }

  private function extractPathSku($path)
  {
    preg_match('@.*/(.*)(-\d|_\d)@', $path, $m);
    return $m[1];
  }

  private function getSkusToMap()
  {
    $ffSeed = new \Oerp\Gallery\File\Finder\Gallery('seed');
    $seeds = $ffSeed->getList('*.jpg');

    $missing = array();
    foreach($seeds as $ind => $seed){
      $sku = $this->extractPathSku($seed);
      unset($seeds[$ind]);

      if(isset($missing[$sku])){
        continue;
      }
      else{
        $missing[$sku] = true;
      }
    }

    $ffPublish = new \Oerp\Gallery\File\Finder\Gallery('publish/All');
    $publish = $ffPublish->getList('*.jpg');

    foreach($publish as $ind => $path){
      $sku = $this->extractPathSku($path);
      unset($publish[$ind]);
      $missing[$sku] = false;
    }

    return array_keys(array_filter($missing));
  }

  private function getEleSelectAccount()
  {
    $ff = new \Oerp\Gallery\File\Finder\Gallery('watermark');
    $list = $ff->getList('*-0-*.png', true);
    preg_match_all('@.*/(.*)-0-.*.png@', $list, $m);

    $entries = array();
    $entries[] = l('New', 'oerp/gallery/mapping/New');
    $entries[] = l('All', 'oerp/gallery/mapping/All');

    foreach($m[1] as $account){
      if($account != 'All'){
        $entries[] = l($account, 'oerp/gallery/mapping/' . $account);
      }
    }

    $src = sprintf(
      '<div class="select-account">%s</div>',
      theme('item_list', $entries, null, 'ul', array('class' => 'select-account'))
    );

    return array(
      '#type' => 'markup',
      '#value' => $src,
    );
  }

  protected function hook_form_internal()
  {
    $form = array();

    list($this->accnt) = $this->args;
    $form['select-account'] = $this->getEleSelectAccount();

    if($this->accnt == 'New'){
      $this->accnt = 'All';
      $this->setElesMapping($form, $this->getSkusToMap(), array('0', '1', '2', '3'));
    }
    else{
      $this->setElesSearch($form);

      $skus = $this->getDefaultValue('sku');
      $suffixes = $this->getDefaultValue('suffix');

      if($skus && !is_null($suffixes)){
        $this->setElesMapping($form, $skus, $suffixes);
      }
    }

    $form['accnt'] = array(
      '#type' => 'value',
      '#value' => $this->accnt,
    );

    return $form;
  }

  private function getSeedPool($sku){
    $ff = new \Oerp\Gallery\File\Finder\Gallery('seed');
    $pat = $sku . '*.jpg';

    $seed_src = '';
    foreach($ff->search($pat) as $pic){
      $seed_src .= sprintf(
        '<div class="pool-slot"><img class="seed %s" src="%s"/></div>',
        'sku-' . $sku,
        $pic . '?' . $this->ts
      );
    }

    $buf = sprintf('<div class="pool">%s<div class="clearfix"></div></div>', $seed_src);
    return $buf;
  }

  private function getMappingSlot(&$form, $name)
  {
    $path_null = drupal_get_path('module', 'oerp_gallery') . '/images/null.png';

    $attrs = $form[$name]['#attributes'];
    $mapped = $attrs['mapped'];
    $queued = $attrs['queued'];

    $path = $queued ? $queued :
      ($mapped ? $mapped : $path_null);

    $slot_src = sprintf('<div class="mapping-sel"><img src="%s"/></div>', $path . '?' . $this->ts);

    return sprintf(
      '<div class="mapping-slot %s %s" sku="%s"><h2>%s</h2>%s%s%s</div>',
      $mapped ? 'occupied' : '',
      $queued ? 'queued' : '',
      $form[$name]['#sku'],
      $form[$name]['#fn'],
      drupal_render($form[$name]),
      drupal_render($form['del-' . $name]),
      $slot_src
    );
  }

  public function hook_theme($form)
  {
    $tpl = <<<HTML
<div class="yui-t2">
  <div id="yui-main">
    <div class="yui-b groups">%s</div>
  </div>
  <div class="yui-b">%s</div>
</div>
HTML;

    $buf_left = '';
    $buf_left .= drupal_render($form['select-account']);
    $buf_left .= drupal_render($form['sku']);
    $buf_left .= drupal_render($form['suffix']);
    $buf_left .= drupal_render($form['search']);

    $groups = array();

    foreach($form as $name => $ele){
      if(strpos($name, self::ELEPREFIX) === 0){
        $sku = $ele['#sku'];
        $groups[$sku][] = $this->getMappingSlot($form, $name);
      }
    }

    $buf_right = '';

    foreach($groups as $sku => $group){
      $buf_right .= '<div class="group">';
      $buf_right .= '<div class="mapping-slots">';

      foreach($group as $slot_src){
        $buf_right .= $slot_src;
      }

      $buf_right .= '</div>';
      $buf_right .= '<div class="clearfix"></div>';
      $buf_right .= $this->getSeedPool($sku);
      $buf_right .= '</div>';
    }

    $buf_right .= sprintf('<div class="pager"><div class="prev navi">%s</div><div class="next navi">%s</div></div>',
      drupal_render($form['page-prev']),
      drupal_render($form['page-next'])
    );

    $buf_right .= '<div class="clearfix"></div>';
    $buf_right .= drupal_render($form['submit']);

    $buf = sprintf($tpl, $buf_right, $buf_left);
    $buf .= drupal_render($form);

    drupal_add_css(drupal_get_path('module', 'oerp_gallery') . '/oerp_gallery.css');
    jquery_ui_add(array('ui.draggable', 'ui.droppable'));
    drupal_add_js(drupal_get_path('module', 'oerp_gallery') . '/mapping.js');

    return $buf;
  }

  protected function getToolbarBottom()
  {
    return array();
  }
}