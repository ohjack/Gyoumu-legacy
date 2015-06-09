<?php
namespace Oerp\View;

class Form
{
  private $ele;
  private $param;

  public function __construct($ele)
  {
    $this->ele = $ele;
    $this->param = $ele['#param'];
  }

  private function getParamState()
  {
    $param = $this->param;
    $arch = new \Gulei\Arch\Element($param['arch']);
    $eleForm = $arch->getArch();
    $eleForm->setAttribute('col', 2);

    $btns_src = '';
    foreach ($param['state'] as $state) {
      $btns_src .= sprintf(
        '<button name="%s" string="%s" type="state"/>',
        $state[0], $state[1]
      );
    }

    $EleToolbar = new \Gulei\Arch\Element(
      '<group colspan="2" col="4">' . $btns_src . '</group>'
    );

    $eleToolbar = $eleForm->ownerDocument->importNode(
      $EleToolbar->getArch(), true);

    $eleForm->appendChild($eleToolbar);

    $param['arch'] = $arch->dump();
    return $param;
  }

  private function prepareVid(&$param)
  {
    $param['vid'] = (int)$param['vid'];
  }

  private function prepareModel(&$param)
  {
    if (!$param['model']) {
      $q = new \Oerp\Query\Basic(
        'ir.ui.view', null, array('model'), array($param['vid']));
      $resp = $q->raw();
      $param['model'] = $resp[0]['model'];
    }
  }

  private function prepareType(&$param)
  {
    if (!$param['type']) {
      $param['type'] = 'form';
    }
  }

  private function loadView(&$param)
  {
    if (!$param['arch']) {
      $q = new \Oerp\Query\Basic($param['model']);
      $param['vid'] = (!$param['vid']) ? false : $param['vid'];
      $view = $q->fields_view_get($param['vid'], $param['type']);

      if (!$view['view_id'] && !$view['arch']) {
        throw new Exception(
          'OerpPort: No ' . $param['type'] . ' view ID nor arch source is found for ' . $param['model'] . '".');
      }

      $param['name'] = $view['name'];
      $param['vid'] = $view['view_id'];
      $param['arch'] = $view['arch'];
      $param['fields'] = $view['fields'];
    }
  }

  private function prepareFields(&$param)
  {
    if (!$param['fields']) {
      $q = new \Oerp\Query\Basic($param['model']);
      $resp = (array)$q->getDef();
      $param['fields'] = $resp[0];
    }

    $param = $this->mergeWithHook($param);

    if(isset($param['override_fields'])){
      $param['fields'] = array_merge_recursive_simple(
        $param['fields'], $param['override_fields']);
    }
  }

  private function prepareArch(&$param)
  {
    $Arch = new \Gulei\Arch\Element($param['arch']);

    foreach($param['fields'] as $name => $field){
      if($field['invisible']){
        if($Fld = $Arch->xp('//field[@name="' . $name . '"]')->item(0)){
          $Fld->setAttribute('invisible', 1);
        }
      }
    }
    $param['arch'] = $Arch->dump();
  }

  private function prepareKey(&$param)
  {
    if (!$param['key']) {
      $param['key'] = $this->ele['#name'];
    }
  }

  private function prepareRid(&$param)
  {
    $param['rid'] = (int)$param['rid'];
  }

  private function getParam()
  {
    $param = $this->param;

    if ($param['state']) {
      return $this->getParamState();
    }

    if (!$param['model'] && !$param['vid']) {
      throw new Exception('OerpPort: No model name nor view ID provided.');
    }

    $this->prepareVid($param);
    $this->prepareModel($param);
    $this->prepareType($param);

    $this->loadView($param);
    $this->prepareFields($param);
    $this->prepareArch($param);
    $this->prepareKey($param);
    $this->prepareRid($param);
    return $param;
  }

  static public function getPassAttrs()
  {
    return array('on_change', 'domain');
  }

  private function getHook($name = false)
  {
    $all_hooks = module_invoke_all('oerp_formview_fields');
    $hook = ($name === false) ? $all_hooks : $all_hooks[$name];
    return empty($hook)  ? array() : $hook;
  }

  private function mergeWithHook($param)
  {
    $hook = $this->getHook($param['name']);
    $Arry = new \Gulei\_Array\Basic($param['fields']);
    $param['fields'] = $Arry->exe('mergeRecursively', $hook);
    return $param;
  }

  private function getDefaultValue($param)
  {
    $isWiz = !!$param['state'];
    if (!$isWiz) {
      if ($param['rid'] == 0) {
        $q = new \Oerp\Query\Basic($param['model']);
        $default_val = $q->default_get(array_keys($param['fields']));
        $param = $this->mergeWithHook($param);

        foreach($param['fields'] as $fld_name => $def){
          if(isset($def['default_value'])){
            $default_val[$fld_name] = $def['default_value'];
          }
          else if(isset($def['value'])){
            $default_val[$fld_name] = $def['value'];
          }
        }

        if(isset($param['override_fields'])){
          foreach($param['override_fields'] as $name => $over_field){
            if(isset($over_field['default_value'])){
              $default_val[$name] = $over_field['default_value'];
            }
          }
        }
      }
      else {
        $q = new \Oerp\Query\Basic(
          $param['model'], null, null, array((int)$param['rid']));

        $resp = $q->raw(false, true);
        $default_val = $resp[0];
      }
    }
    else {
      $default_val = array();
      foreach ($param['fields'] as $name => $field) {
        $default_val[$name] = $field['value'];
      }
    }
    return $default_val;
  }

  private function prepareEleEntries(&$eles)
  {
    $param = $eles['#param'];
    $Arch = new \Gulei\Arch\Element($param['arch']);
    $default_val = $this->getDefaultValue($param);

    foreach ($param['fields'] as $fld_name => $fld) {
      $eleFld = $Arch->xp('.//field[@name="' . $fld_name . '"]')->item(0);

      $args = array(
        'name' => $fld_name,
        'field' => $fld,
        'arch' => $Arch->dump($eleFld),
        'default' => $default_val[$fld_name],
        'model' => $param['model'],
        'rid' => $param['rid'],
        'key' => $param['key'],
        'state' => $default_val['state']
      );

      $ele = \Oerp\Form\Element\Factory::createElement($args)->getEntry();
      $eles[$fld_name] = $ele;
    }

    if ($default_val['state']) {
      $eles['oerp:state'] = array(
        '#type' => 'hidden',
        '#value' => '',
        '#attributes' => array(
          'state' => $default_val['state']
        ),
      );
    }
  }

  private function prepareEleButtons(&$eles)
  {
    $param = $eles['#param'];
    $Arch = new \Gulei\Arch\Element($param['arch']);

    foreach ($Arch->xp('//button') as $eleButton) {
      $name = $eleButton->getAttribute('name');
      $btn_type = $eleButton->getAttribute('type');

      $eles[$name] = array(
        '#type' => 'submit',
        '#name' => $name,
        '#value' => $eleButton->getAttribute('string'),
        '#param' => array(
          'type' => $eleButton->getAttribute('type'),
        ),
        '#attributes' => array(
          'model' => $param['model'],
          'rid' => $param['rid'],
          'wid' => $param['wid'],
          'datas' => json_encode($param['datas']),
          'button_type' => $btn_type ? $btn_type : 'workflow',
        )
      );
    }
  }

  private function getEles()
  {
    $eles = array();
    $eles['#tree'] = true;
    $eles['#param'] = $this->getParam();

    $this->prepareEleEntries($eles);
    $this->prepareEleButtons($eles);
    return $eles;
  }

  public function process($ele, $edit, $form_state, $form)
  {
    $ele = array_merge($ele, $this->getEles());
    $ele['#build_id'] = $form['#build_id'];
    return $ele;
  }

  public function theme()
  {
    \Gulei\Form\Basic::addJs();
    \Gulei\Form\Basic::addCss();

    \Oerp\Util\Helper::addJs('jquery.oerpview', 'formview');
    \Oerp\Util\Helper::addCss();

    $eles = $this->ele;
    $param = $eles['#param'];
    $key = ($param['key']) ? $param['key'] : $eles['#name'];

    $w = new \Oerp\View\Wrapper($param['fields'], $param['arch'], $key);
    $src = $w->renderForm($eles['#children'], $eles['#build_id']);

    preg_match_all('@(<textarea [^<]*)/>@', $src, $m);

    if($m){
      foreach($m[0] as $seq => $each){
        $src = str_replace($each, $m[1][$seq] . '></textarea>', $src);
      }
    }

    return sprintf(
      '
      <div class="oerp-view %s %s" oerp_vid="%s" oerp_model="%s" oerp_rid="%s">
        <div class="oerp-widget-content ui-widget-content">%s</div>
      </div>
      ',
      $param['prompt'] ? 'prompt' : '', $param['new'] ? 'new' : '',
      $param['vid'], $param['model'], $param['rid'], $src
    );
  }
}