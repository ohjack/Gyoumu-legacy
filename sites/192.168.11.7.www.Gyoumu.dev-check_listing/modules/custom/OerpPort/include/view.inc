<?php
use Gulei as G;
use Oerp as O;

module_load_include('inc', 'GuLib', '/include/GuTable');
module_load_include('inc', 'GuLib', '/include/page');
require_once 'modifier.inc';
require_once 'provider.inc';

class OerpWrapper extends GuWrapper
{
  protected $arch;
  protected $fields;
  protected $key;

  function __construct($fields, $arch, $key = '', $col = 4)
  {
    $this->arch = new GuArchElement($arch);
    $this->fields = $fields;
    $this->key = $key;

    if ($col_apply = $this->arch->getArch()->getAttribute('col')) {
      parent::__construct($col_apply);
    }
    else {
      parent::__construct($col);
    }
  }

  static protected function getOpts($arch, $col)
  {
    $opts = array();
    $opts['colspan'] = (int)$arch->getAttribute('colspan');

    if ($opts['colspan']) {
      if ($opts['colspan'] > $col) {
        $opts['colspan'] = $col;
      }
    }
    else {
      switch ($arch->nodeName) {
        case 'form':
        case 'tree':
        case 'page':
        case 'separator':
          $opts['colspan'] = $col;
          break;

        case 'group':
        case 'notebook':
        case 'label':
          $opts['colspan'] = 1;
          break;

        case 'field':
        case 'button':
          $opts['colspan'] = 2;
          break;
      }
    }
    return $opts;
  }

  public function renderForm($ele_src, $build_id)
  {
    $arch = $this->arch->getArch();
    $ele_src = '<div>' . $ele_src . '</div>';
    $EleSrc = new GuArchElement($ele_src);

    $eleState = $EleSrc->xp('//input[@type="hidden"][@state][1]')->item(0);
    if ($eleState) {
      $state = $eleState->getAttribute('state');
    }
    unset($eleState);

    foreach ($this->arch->xp('./*', $this->arch->getArch()) as $fld) {
      $ftag = $fld->nodeName;

      if ($ftag == 'newline') {
        $this->addBreak();
        continue;
      }

      if ($fld->getAttribute('invisible')) {
        $name = $fld->getAttribute('name');
        $key = str_replace('_', '-', $name);
        $key = ($this->key) ? $this->key . '-' . $key : $key;

        $Ele = $EleSrc->xp('//input[@id="edit-' . $key . '"]')->item(0);
        $this->addSuffix(
          sprintf(
            '<div oerp_name="%s" oerp_type="%s" invisible="1">%s</div>',
            $name, $this->fields[$name]['type'], $EleSrc->dump($Ele)
          )
        );

        unset($Ele);
        continue;
      }

      $opts = OerpWrapper::getOpts($fld, $this->col);

      if ($fld->getAttribute('nolabel')) {
        $opts['class'] .= ' nolabel';
      }

      switch ($ftag) {
        case 'label':
          $this->addItem($fld->getAttribute('string'), $opts);
          break;

        case 'button':
          $name = $fld->getAttribute('name');

          $states = $fld->getAttribute('states');
          $states = explode(',', $states);
          array_walk($states, 'trim');

          if ($states && !in_array($state, $states)) {
            continue;
          }

          $eleButton =
              $EleSrc->xp('//input[@type="submit"][@name="' . $name . '"][1]')->item(0);

          $this->addItem(sprintf('
            <div class="form-item">
              <div class="field">
                <label><sup>&nbsp;</sup></label>
                %s
              </div>
            </div>
            ', $EleSrc->dump($eleButton)), $opts);
          break;

        case 'form':
        case 'tree':
        case 'field':
        case 'group':
        case 'notebook':
        case 'page':
        case 'separator':
          $innerCol = (int)$fld->getAttribute('col');
          $innerCol = $innerCol ? $innerCol : 4;
          $InnerWrapper = new OerpWrapper($this->fields, $fld, $this->key, $innerCol);
          $this->addItem($InnerWrapper->renderForm($ele_src, $build_id), $opts);
          break;
      }
    }

    switch ($arch->nodeName) {
      case 'separator':
        return sprintf('<div class="separator caption">%s</div>',
                       $arch->getAttribute('string'));

      case 'field':
        $key = str_replace('_', '-', $arch->getAttribute('name'));
        $key = ($this->key) ? $this->key . '-' . $key : $key;

        $snippet = $EleSrc->xp('//div[@id="edit-' . $key . '-wrapper"]')->item(0);

        //      handle readonly fields
        if ($arch->getAttribute('readonly')) {
          foreach ($EleSrc->xp('./input | ./textarea', $snippet) as $innerInput) {
            $innerInput->setAttribute('readonly', 'readonly');
          }

          foreach ($EleSrc->xp('./select', $snippet) as $innerInput) {
            $innerInput->setAttribute('disabled', 'disabled');
          }
        }

        //      workaround for <textarea/>
        if (!$snippet) {
          dpr($key);
          dpr($ele_src);
          echo '<pre>';
          throw new Exception('Field element not found.');
        }

        $snippet = $EleSrc->dump($snippet);
        $snippet = str_replace('%DUMMY%', '', $snippet);

        $name = $arch->getAttribute('name');
        $type = $this->fields[$name]['type'];

        return sprintf('<div class="field" oerp_type="%s" oerp_name="%s" id="oerp-field-%s">%s</div>',
                       $type, $name, $key, $snippet);

      case 'notebook':
        $index = '';
        foreach ($this->arch->xp('./page', $arch) as $page) {
          $cap = $page->getAttribute('string');
          $index .= sprintf('<li><a href="#%s">%s</a></li>',
                            'oerp-tab-' . md5($build_id . $cap), $cap);
        }

        $output = sprintf('
          <div class="notebook" id="oerp-tabs-%s">
            <ul>%s</ul>
            %s
          </div>
          ', md5(microtime()), $index, $this->render());

        return $output;

      case 'page':
        $cap = $arch->getAttribute('string');
        return sprintf('<div class="page" id="%s">%s</div>',
                       'oerp-tab-' . md5($build_id . $cap), $this->render());

      default:
        return $this->render();
    }
  }
}

function _OerpViewSnippet_form(&$form_state, $eles)
{
  return array('view' => $eles);
}

class OerpFormView
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
    $arch = new GuArchElement($param['arch']);
    $eleForm = $arch->getArch();
    $eleForm->setAttribute('col', 2);

    $btns_src = '';
    foreach ($param['state'] as $state) {
      $btns_src .= sprintf(
        '<button name="%s" string="%s" type="state"/>',
        $state[0], $state[1]
      );
    }

    $EleToolbar = new GuArchElement(
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
      $q = new OerpQuery(
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
      $q = new OerpQuery($param['model']);
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
      $q = new OerpQuery($param['model']);
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
    $Arch = new GuArchElement($param['arch']);

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
    $param['fields'] = array_merge_recursive_simple($param['fields'], $hook);
    return $param;
  }

  private function getDefaultValue($param)
  {
    $isWiz = !!$param['state'];
    if (!$isWiz) {
      if ($param['rid'] == 0) {
        $q = new OerpQuery($param['model']);
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
        $q = new OerpQuery(
          $param['model'], null, null, array((int)$param['rid']));

        $resp = $q->raw();
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
    $Arch = new GuArchElement($param['arch']);
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

      $ele = O\FormElementFactory::createElement($args)->getEntry();
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
    $Arch = new GuArchElement($param['arch']);

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
    GuForm::addJs();
    GuForm::addCss();

    OerpUtil::addJs('jquery.oerpview', 'formview');
    OerpUtil::addCss();

    $eles = $this->ele;
    $param = $eles['#param'];
    $key = ($param['key']) ? $param['key'] : $eles['#name'];

    $w = new OerpWrapper($param['fields'], $param['arch'], $key);
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

class OerpTreeView extends AbstractTable
{
  protected function initParam($eles)
  {
    //assign passed values
    $param = $eles['#param'];

    //query db if any needed value is missing
    if( !$param['vid'] && !$param['model'] && $param['arch']) {
      $param = $this->initParamNoVid($param);
    }
    else {
      $param = $this->initParamHasVid($param);
    }

    $param['key'] = ($param['key']) ? $param['key'] : $eles['#name'];

    $param['criteria'] = is_string($param['criteria'])
        ? json_decode($param['criteria']) : $param['criteria'];

    $param['criteria'] = is_array($param['criteria'])
        ? $param['criteria'] : array();

    if ($param['cri_url']) {
      $cri = OerpSearchPanelController::decodeCri($param['cri_url']);
      $cri = is_array($cri) ? $cri : array();

      $param['criteria'] = array_merge($param['criteria'], $cri);
    }
    return $param;
  }

  function __construct($eles)
  {
    $param = array_merge(
      $this->getDefaultParam(),
      $this->initParam($eles)
    );

    parent::__construct(
      $param['arch'],
      new G\ProviderArch($param['arch']),
      new O\ProviderTreeRecords($param),
      $param
    );

    $this->mods['hd']['raw'] = array(new OerpPackHeaderRawMods());
    $this->mods['row']['raw'] = array(new OerpPackRowRawMods());
  }

  protected function getFuncPrefix()
  {
    return array('GuFunc_', 'OerpFunc_');
  }

  protected function initParamHasVid($param)
  {
    $model = $param['oerp:model'] ? $param['oerp:model'] : $param['model'];

    if (!$param['fields'] || !$param['arch']) {
      $q = new OerpQuery($model);
      $view = $q->fields_view_get($param['vid'], 'tree');

      if (is_array($param['fields'])) {
        $param['fields'] = array_intersect($view['fields'], $param['fields']);
      }
      else {
        $param['fields'] = $view['fields'];
      }

      $param['arch'] = $view['arch'];
    }
    return $param;
  }

  protected function initParamNoVid($param)
  {
    $Arch = new GuArchElement($param['arch']);
    $OerpParam = $Arch->getNodeValues('/tree/param[@name]');

    if ($OerpParam['oerp:model']) {
      $param['model'] = $OerpParam['oerp:model'];
    }

    if ($OerpParam['oerp:criteria']) {
      $param['criteria'] = $OerpParam['oerp:criteria'];
    }

    if ($Arch->xp('/tree/field/field[1]')->length) {
      $param['nested'] = true;
    }
    return $param;
  }

  protected function getFields()
  {
    $flds = array();
    foreach ($this->xp('/tree/field') as $fld) {
      $flds[] = $fld->getAttribute('name');
    }
    return $flds;
  }

  public function processElement($eles)
  {
    $eles['#obj'] = $this->save();
    $eles['#tree'] = true;
    $eles['json'] = array(
      '#type' => 'hidden',
      '#value' => '',
    );

    return $eles;
  }

  protected function getDefaultParam()
  {
    return array(
      'mark_translated' => true,
      'mark_invalid' => true,
      'mark_notice' => true,
      'mark_missing' => true,
      'copy_method' => 'ditto',
      'nl2br' => true,
      'items_per_page' => 80,
      'sticky_header' => true,
      'mark_translated' => false,
    );
  }

  public function theme($eles, $param = null)
  {
    $param = $this->getParam($param);

    $rows = $this->getRows();
    $tb_class = 'gu-table oerp-treeview ' . $param['oerp:op-type'];
    $tb_class .= $this->getRowCount() > 1 ? '' : ' empty';
    $param['attrs']['class'] .= $tb_class;

    $pat = '
      <div class="oerp-view oerp-treeview" oerp_vid="%s" oerp_key="%s"
        oerp_model="%s" oerp_name="%s" cache_id="%s">

        <div class="caption">%s</div>
        %s
      </div>
    ';

    $model = ($param['oerp:model']) ? $param['oerp:model'] : $param['model'];

    $output = sprintf(
      $pat, '', $param['key'],
      $model, $param['name'], $param['cache_id'],
      $this->getArch()->getAttribute('string'),
      $eles['#children'] . parent::theme(null, $rows, $param)
    );

    IncluderOerpView::includeCss();
    IncluderOerpView::includeJs();
    return $output;
  }

  protected function getSearchOption()
  {
    $q = new OerpQuery($this->getParam('model'));
    $def = $q->exec(
      array('fields_view_get', false, 'form', array())
    );

    $arch = new GuArchElement($def['arch']);
    $search = array();
    /*
    foreach($arch->xp('//field[@context]') as $field){
    $name = $field->getAttribute('name');
    $search['basic'][$name] = $def['fields'][$name];
    }

    foreach($def['fields'] as $name => $field){
    if($field['select'] == 1){
    $search['basic'][$name] = $field;
    }
    }*/

    foreach ($arch->xp('//field[@select="1"]') as $field) {
      $name = $field->getAttribute('name');
      $search['basic'][$name] = $def['fields'][$name];
    }

    foreach ($arch->xp('//field[@select="2"]') as $field) {
      $name = $field->getAttribute('name');
      $search['advanced'][$name] = $def['fields'][$name];
    }

    return $search;
  }

  public function getSearchEntry()
  {
    $param = $this->getParam();
    $arch = new GuArchElement($param['arch']);
    $search = $this->getSearchOption();

    if (!$search['basic']) {
      return false;
    }

    $eles = array();
    $default = OerpSearchPanelController::decodeCri($param['cri_url'], true);

    foreach ($search['basic'] as $name => $fld_def) {
      $node = $arch->xp('/tree/field[@name="' . $name . '"][1]')->item(0);
      $node = $node ? $node : '<field name="' . $name . '"/>';

      $ele = OerpFields::getSearchEle($fld_def, $node, $default[$name]);
      $eles[$name] = $ele;
    }

    $entry['#type'] = 'oerp_searchpanel';
    $entry['#fields'] = $search['basic'];
    $entry['#eles'] = $eles;
    return $entry;
  }


}

class OerpSearchPanelController
{
  static public function process($eles, $edit, $form_state, $complete_form)
  {
    $flds = $eles['#eles'];
    unset($eles['#eles']);

    $key = implode('-', $eles['#parents']);
    $key = str_replace('_', '-', $key);
    $key .= $key ? '-' : '';

    $flds['func:find'] = array(
      '#type' => 'submit',
      '#value' => 'Find',
      '#name' => 'func:find',
      "#prefix" => '<div class="form-item" id="edit-' . $key . 'func:find-wrapper">',
      '#suffix' => '</div>',
    );

    $flds['func:clear'] = array(
      '#type' => 'submit',
      '#value' => 'Clear',
      '#name' => 'func:clear',
      "#prefix" => '<div class="form-item" id="edit-' . $key . 'func:clear-wrapper">',
      '#suffix' => '</div>',
    );

    $eles = array_merge($eles, $flds);
    $eles['#build_id'] = $complete_form['#build_id'];

    return $eles;
  }

  static public function theme($eles)
  {
    if (!$eles['#fields']) {
      return '';
    }

    $arch = '';
    foreach ($eles['#fields'] as $name => $fld) {
      switch ($fld['type']) {
        case 'float':
        case 'date':
        case 'datetime':
          $arch .= sprintf('
            <group colspan="1" col="4">
              <field name="%s-GE"/>
              <field name="%s-LE"/>
            </group>
          ', $name, $name);
          break;

        default:
          $arch .= '<field name="' . $name . '" colspan="1"/>';
      }
    }
    $arch .= '<newline/>
      <group colspan="1" col="4">
        <field type="submit" name="func:find"/>
        <field type="submit" name="func:clear"/>
      </group>
    ';
    $arch = '<form>' . $arch . '</form>';

    $key = implode('-', $eles['#parents']);
    $key = str_replace('_', '-', $key);

    $w = new OerpWrapper($eles['#fields'], $arch, $key, 5);
    return sprintf('<div class="searchpanel">%s</div>',
                   $w->renderForm($eles['#children'], $eles['#build_id']));
  }

  static public function submit($form, &$form_state, $eleSearch, $search)
  {
    $eleSearch = $eleSearch['_search'];
    $search = $search['_search'];
    $btn = $form_state['clicked_button']['#name'];

    switch ($btn) {
      case 'func:find':
        $cri = self::encodeCri(
          $search,
          $eleSearch['#fields']
        );

        $form_state['redirect'] = array($_GET['q'], 'cri=' . $cri);
        return false;

      case 'func:clear':
        $form_state['redirect'] = array($_GET['q']);
        return false;
    }
    return true;
  }

  static public function encodeCri($search, $def)
  {
    if (!is_array($search)) {
      return array();
    }

    $cri = array();

    foreach ($search as $name => $kval) {
      if (strpos($name, 'func:') === 0) {
        continue;
      }

      switch ($def[$name]['type']) {
        case 'char':
        case 'many2one':
        case 'one2many':
          if (strlen($kval)) {
            $cri[] = array($name, 'ilike', rawurlencode($kval));
          }
          break;

        case 'int':
          $cri[] = array($name, '=', (int)$kval);
          break;

        case 'float':
          $min = (float)$kval['GE'];
          $max = (float)$kval['LE'];

          if ($min == $max) {
            if ($min != 0) {
              $cri[] = array($name, '=', $min);
            }
          }
          else {
            $cri[] = array($name, '>=', $min);
            $cri[] = array($name, '<=', $max);
          }
          break;

        case 'date':
        case 'datetime':
          if ($kval['GE']) {
            $cri[] = array($name, '>=', $kval['GE']);
          }

          if ($kval['LE']) {
            $cri[] = array($name, '<=', $kval['LE']);
          }
          break;

        case 'boolean':
          switch ($kval) {
            case 'yes':
              $cri[] = array($name, '=', 1);
              break;

            case 'no':
              $cri[] = array($name, '=', 0);
              break;
          }
          break;

        case 'selection':
          if ($kval == '_null_') {
            continue;
          }
          $cri[] = array($name, '=', $kval);
          break;
      }
    }
    return json_encode($cri);
  }

  static public function decodeCri($str, $asAssoc = false)
  {
    $cri = json_decode($str);

    if (!is_array($cri)) {
      return array();
    }

    if ($asAssoc) {
      $assoc = array();

      foreach ($cri as $each) {
        $assoc[$each[0]][$each[1]] = $each[2];
      }
      return $assoc;
    }
    else {
      return $cri;
    }
  }
}

class OerpViewTreePageController extends FormPageController
{
  private $search_path = array('view', '_search');

  public function hook_form(&$form_state, $vid, $param = array())
  {
    $this->redirect($form_state);

    $param = array_merge(
      array('oerp:op-type' => 'edit'),
      $param
    );

    $form = array();
    $form['#attributes'] = array('class' => 'oerp-formview');

    $param['vid'] = $vid;

    if ($param['vid'] && !$param['model']) {
      $q = new OerpQuery('ir.ui.view', null, array('model'), $vid);
      $rec = $q->raw();
      $param['model'] = $rec[0]['model'];
    }

    $param['searchpanel'] = true;
    $param['cri_url'] = $param['cri_url'] ? $param['cri_url'] : $_GET['cri'];

    $search = $_GET;
    unset($search['q'], $search['page']);

    if (!isset($param['pager'])) {
      $param['pager'] = true;
    }

    $form['view'] = array(
      '#type' => 'oerp_treeview',
      '#param' => $param,
    );

    $form['func:submit'] = array(
      '#type' => 'submit',
      '#value' => 'Save',
      '#name' => 'func:submit',
    );

    $form['func:downloadCsv'] = array(
      '#type' => 'submit',
      '#value' => 'Download',
      '#name' => 'func:downloadCsv',
    );

    return $form;
  }

  protected function proc_view__search_func_find($form, &$form_state)
  {
    $search = $form_state['values']['view'];
    $eleSearch = $form['view'];

    if (!OerpSearchPanelController::submit(
      $form, $form_state, $eleSearch, $search)
    ) {
      return;
    }
    
    $this->goBack(false, $this->search_path);
  }

  protected function proc_view__search_func_clear()
  {
    $this->goBack();
  }

  protected function proc_func_submit($form, &$form_state)
  {
    $recs = OerpUtil::jsondecode(
      $form_state['clicked_button']['#post']['view']['json']);

    $recs = OerpUtil::renderNestedInput($recs);
    $param = $form['view']['#obj']->renew()->getParam();

    foreach ($recs as $rec) {
      $q = new OerpQuery($param['model'], null, null, array($rec[1]));

      switch ($rec[0]) {
        case 0: //create
          $q->create($rec[2]);
          break;

        case 1: //edit
          $q->write($rec[2]);
          break;

        case 2: //delete
          $q->unlink();
          break;
      }
    }
    $this->goBack();
  }

  protected function proc_func_downloadCsv($form, &$form_state)
  {
    $obj = $form['view']['#obj']->renew();
    $fname = 'Treeview-' . str_replace('.', '_', $obj->getParam('oerp:model'));

    $param = array(
      'oerp:op-type' => null,
    );

    $form_state['storage']['redirect'] = $obj->getCsvFile($param, $fname);
  }
}

function oerp_ViewTree_form(&$form_state, $vid, $param = array())
{
  $p = new OerpViewTreePageController($form_state);
  return $p->hook_form($form_state, $vid, $param);
}

function oerp_ViewTree_form_submit($form, &$form_state)
{
  $p = new OerpViewTreePageController($form_state);
  $p->hook_form_submit($form, $form_state);
}

class OerpViewFormPageController extends FormPageController
{
  public function hook_form(&$form_state, $model, $vid, $rid, $param)
  {
    if ($model && !strpos($rid, '0-') === 0) {
      $q = new OerpQuery($model, null, array('id'), $rid);
      if (!(array)$q->raw()) {
        drupal_set_message('Specified record is not found.', 'error');

        return drupal_goto(
          sprintf(
            'oerpport/view/form/%s/%s/0',
            $model, $vid
          )
        );
      }
    }

    $form = array();
    $form['#attributes'] = array('class' => 'oerp-formview');

    $param = array_merge(
      $param,
      array(
           'model' => $model,
           'vid' => $vid,
           'rid' => $rid,
      )
    );

    $form['view'] = array(
      '#type' => 'oerp_formview',
      '#param' => $param,
    );

    $form['func:submit'] = array(
      '#type' => 'submit',
      '#value' => 'Save',
      '#name' => 'func:submit',
    );

    return $form;
  }

  private function saveForm($form, $form_state)
  {
    $param = $form['view']['#param'];

    //render nested value
    $post = $form_state['clicked_button']['#post']['view'];
    $nested = array_filter($post, 'is_array');

    $nested = array_map(
      create_function(
        '$n',
        'return OerpUtil::jsondecode($n["json"]);'
      ),
      $nested
    );

    $nested = OerpUtil::renderNestedInput($nested);

    //render form value
    $flds = $param['fields'];
    $vals = $form_state['values']['view'];

    foreach ($vals as $key => $val) {
      if ($flds[$key]['readonly']) {
        unset($vals[$key]);
      }
      else {
        $args = array(
          'field' => array('type' => $flds[$key]['type']),
          'value' => $val,
          'nested_value' => $nested[$key]
        );

        $val = O\FormElementFactory::prepareSend($args);

        if(is_null($val)) {
          unset($vals[$key]);
        }
        else {
          $vals[$key] = $val;
        }
      }
    }

    $q = new OerpQuery($param['model'], null, null, array($param['rid']));

    if ($param['rid'] == 0) {
      $resp = $q->create($vals);
    }
    else {
      $resp = $q->write($vals);
    }

    if ($resp === true) {
      return $param['rid'];
    }
    else {
      return $resp;
    }
  }

  protected function proc_func_submit($form, &$form_state)
  {
    $rid = $this->saveForm($form, $form_state);

    if (!$rid) {
      drupal_set_message("Failed to process the record.", 'error');
      return;
    }

    $entries = explode('/', $this->q);
    $entries[count($entries) - 1] = $rid;

    $form_state['redirect'] = implode($entries, '/');
    drupal_set_message('Record saved.');
    return;

    $q = new OerpQuery($param['model']);

    switch ($btn['#param']['type']) {
      case 'object':
        $resp = $q->exec(
          array(
               $btn['#name'],
               array($rid),
          )
        );
        break;

      default:
        $resp = $q->exec_workflow(
          array(
               $btn['#name'],
               $rid,
          )
        );
        break;
    }
  }
}

function oerp_ViewForm_form(&$form_state, $model, $vid, $rid, $param = array())
{
  $ctrl = new OerpViewFormPageController($form_state);
  return $ctrl->hook_form(&$form_state, $model, $vid, $rid, $param);
}

function oerp_ViewForm_form_submit($form, &$form_state)
{
  $ctrl = new OerpViewFormPageController($form_state);
  return $ctrl->hook_form_submit($form, &$form_state);
}

function oerp_ViewEditor_js()
{
  $post = $_POST;

  if ($post['act_id']) {
    $post['act_id'] = (int)$post['act_id'];
    $post['rid'] = (int)$post['rid'];

    $def = OerpQuery::_execute(
      array(
           'ir.actions.actions',
           'read',
           $post['act_id'],
           array('type')
      )
    );

    $act = OerpQuery::_execute(
      array(
           $def['type'],
           'read',
           $post['act_id'],
           false
      )
    );

    switch ($def['type']) {
      case 'ir.actions.wizard':
        $wiz_id = (int)OerpQueryWizard::_execute(
          array(
               $act['wiz_name'],
          ),
          'create'
        );

        $param = OerpQueryWizard::_execute(
          array(
               $wiz_id,
               array(
                 'model' => $post['model'],
                 'form' => array(),
                 'id' => $post['rid'],
               ),
               'init'
          ),
          'execute'
        );

        $param['wid'] = $wiz_id;
        break;
    }

    $src = drupal_get_form(
      'oerp_ViewForm_form', $post['model'], 0, $post['rid'], $param
    );
    return drupal_json(rawurlencode($src));
  }

  if ($post['cache_id']) {
    $cache = cache_get($post['cache_id'], 'cache_oerpport')->data;

    $param = array(
      'fields' => $cache['fields'],
      'arch' => $cache['arch'],
      'prompt' => true,
      'new' => !$post['rid']
    );

    $src = drupal_get_form(
      'oerp_ViewForm_form', $post['model'], 0, $post['rid'], $param);

    return drupal_json(rawurlencode($src));
  }

  if ($post['par_model']) {
    $q = new OerpQuery($post['par_model']);
    $resp = $q->fields_view_get($post['par_vid'], 'form');
    $field = $resp['fields'][$post['name']];
    unset($resp);

    if ($form = $field['views']['form']) {
      $param = array(
        'fields' => $form['fields'],
        'arch' => $form['arch'],
        'prompt' => true,
        'new' => !$post['rid']
      );

      $src = drupal_get_form(
        'oerp_ViewForm_form', $field['relation'], 0, $post['rid'], $param);

      return drupal_json(rawurlencode($src));
    }
  }

  $q = new OerpQuery($post['model']);

  if ($post['vid']) {
    $view = $q->fields_view_get($post['vid'], $post['type']);
  }
  else {
    $view = $q->fields_view_get(false, $post['type']);
  }

  if ($post['type'] == 'form') {
    $param = array(
      'prompt' => true,
      'new' => !$post['rid']
    );

    $src = drupal_get_form(
      'oerp_ViewForm_form', $post['model'], $view['view_id'], $post['rid'], $param);

    return drupal_json(rawurlencode($src));
  }
  else if ($post['type'] == 'tree') {
    if ($post['domain']) {
      $domain = strtr(
        $post['domain'],
        array(
             "'" => '"',
             '(' => '[',
             ')' => ']',
        )
      );

      $tmpDomain = json_decode($domain);
      $tmpDomain = is_array($tmpDomain) ? $tmpDomain : array();

      $cri = array();
      foreach ($tmpDomain as $each) {
        $cri[$each[0]][$each[1]] = $each[2];
      }
      unset($tmpDomain);

      foreach ($cri as $name => $each) {
        if (isset($each['>=']) && isset($each['<='])) {
          $min = (float)$each['>='];
          $max = (float)$each['<='];

          if ($min == $max) {
            if ($min == 0) {
              unset($cri[$name]);
            }
            else {
              $cri[$name] = array('=' => (string)$min);
            }
          }
        }
      }

      $domain = array();
      foreach ($cri as $name => $exp) {
        foreach ($exp as $op => $val) {
          if ($val != '_null_') {
            $domain[] = array($name, $op, $val);
          }
        }
      }
      $domain = json_encode($domain);
    }

    $param = array(
      'model' => $post['model'],
      'name' => $post['name'],
      'criteria' => $domain,
      'cri_url' => $domain,
      'oerp:op-type' => 'select-one',
      'pager' => false,
      'prompt' => true
    );

    $src = drupal_get_form('oerp_ViewTree_form', $post['vid'], $param);
    $src = '<div style="max-height: 400px; overflow: auto;">' . $src . '</div>';

    return drupal_json(rawurlencode($src));
  }
}

function oerp_formview_process($ele, $edit, &$form_state, $form)
{
  $ctrl = new OerpFormView($ele);
  return $ctrl->process($ele, $edit, $form_state, $form);
}

function theme_oerp_formview($ele)
{
  $ctrl = new OerpFormView($ele);
  return $ctrl->theme();
}

function oerp_treeview_process($eles, $edit, &$form_state, $complete_form)
{
  $v = new OerpTreeView($eles);
  $eles = $v->processElement($eles);

  if ($eles['#param']['searchpanel']) {
    $eles['_search'] = $v->getSearchEntry();
  }
  return $eles;
}

function theme_oerp_treeview($eles)
{
  $v = $eles['#obj']->renew();
  return $v->theme($eles);
}


function oerp_many2one_process($eles, $edit, &$form_state, $complete_form)
{
  $attrs = $eles['#attributes'];

  $edit_class = ($attrs['readonly'] || !$eles['#value']['id'] || $attrs['selectonly'])
      ? 'ui-state-disabled' : '';

  $search_class = ($attrs['readonly'])
      ? 'ui-state-disabled' : '';

  $eles['rid'] = array(
    '#type' => 'hidden',
  );

  $eles['name'] = array(
    '#type' => 'textfield',
    '#title' => $eles['#title'],
    '#value' => $eles['#value']['name'],
    '#description' => $eles['#description'],
    '#required' => $eles['#required'],

    '#attributes' => array(
      'readonly' => '1',
      'selectonly' => $attrs['selectonly'],
      'class' => 'with_op',
      'rid' => $eles['#value']['id'],
      'model' => $eles['#value']['model'],
      'prepareSend' => $attrs['prepareSend'],
    ),

    '#field_suffix' => sprintf('
      <div class="oerp-button oerp-btn-edit %s">
        <span class="oerp-icon ui-icon ui-icon-pencil"></span>
      </div>
      <div class="oerp-button oerp-btn-search %s">
        <span class="oerp-icon ui-icon ui-icon-search"></span>
      </div>
    ', $edit_class, $search_class),
  );

  foreach (OerpFormView::getPassAttrs() as $pass_attr) {
    if ($attr_val = $eles['#attributes'][$pass_attr]) {
      $eles['name']['#attributes'][$pass_attr] = $attr_val;
    }
  }

  return $eles;
}

function theme_oerp_many2one($eles)
{
  preg_match('@<div[^>]*>([\s\S]*)</div>@', $eles['#children'], $m);

  return sprintf('
    <div id="%s-wrapper" class="form-item">%s</div>
  ', $eles['#id'], $m[1]);
}

function oerp_searchpanel_process($eles, $edit, $form_state, $complete_form)
{
  return OerpSearchPanelController::process($eles, $edit, $form_state, $complete_form);
}

function theme_oerp_searchpanel($eles)
{
  return OerpSearchPanelController::theme($eles);
}


class OerpViewController
{
  static public function getCustomTreeForm($module, $filepath){
    $arch = file_get_contents(
      drupal_get_path('module', $module) . $filepath);

    $form = oerp_ViewTree_form($form_state, 0, array('arch' => $arch));
    return $form;
  }
}

class IncluderOerpView implements G\InterfaceIncluder
{
  static public function includeCss()
  {
    drupal_add_css(drupal_get_path('module', 'GuLib') . '/css/table.css');
    GuForm::addCss();
    OerpUtil::addCss();
  }

  static public function includeJs()
  {
    drupal_add_js(drupal_get_path('module', 'GuLib') . '/js/GuTable.js');
    GuForm::addJs();
    OerpUtil::addJs('jquery.oerpview', 'formview', 'json2');
  }
}
