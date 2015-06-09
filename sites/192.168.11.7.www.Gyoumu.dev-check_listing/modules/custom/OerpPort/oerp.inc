<?php
use Oerp as O;

module_load_include('inc', 'GuLib', 'include/GuLib');
module_load_include('inc', 'GuLib', 'include/GuArch');
require_once('include/query.inc');
require_once('include/func.inc');
require_once('include/form.inc');
require_once('include/view.inc');
require_once('include/table.inc');

class OerpArray extends GuArray
{
  public function inherit(&$recs, $prefix = null, &$anchor = null){
    if(empty($recs)){
      $recs[] = array();
      $anchor = &$recs[0];
    }
    $rec = array();

    foreach($this as $name => $each){
      if(is_numeric($name)){
        $inner = new OerpArray($each);

        if((int)$name > 0){
          $recs[] = array();
          $last_ind = count($recs) - 1;
          $recs[$last_ind] = $inner->inherit($recs, $prefix, $recs[$last_ind]);
        }
        else{
          $inner_rec = $inner->inherit($recs, $prefix, $anchor);
          $rec = array_merge($anchor, $inner_rec);

          if(empty($recs[0])){
            $anchor = array_merge($anchor, $rec);
          }
        }
      }
      else{
        $key = empty($prefix) ? $name : $prefix . '.' . $name;

        if(is_array($each)){
          $inner = new OerpArray($each);
          $rec = array_merge($rec, $inner->inherit($recs, $key, $rec));
        }
        else{
          $rec[$key] = $each;
        }
      }
    }

    return $rec;
  }

  public function supplement($fld_names, $method){
    $recs = array();
    $this->inherit($recs);

    if(empty($recs[0])){
      return array();
    }

    $name_chks = array();
    foreach($fld_names as $fld_name){
      $rel = explode('.', $fld_name);
      array_pop($rel);

      if($rel){
        $name_chks[$fld_name] = $rel;
      }
    }

    foreach ($recs as $ind => &$rec) {
      $isRoot = false;
      
      $rec_name_chks = array();
      foreach(array_keys($rec) as $name){
        $rel = explode('.', $name);
        array_pop($rel);

        if($rel){
          $rec_name_chks[] = $rel;
        }
        else{
          $isRoot = true;
          break;
        }
      }

      if($isRoot){
        continue;
      }

      foreach ($fld_names as $fld_name) {
        if(!isset($rec[$fld_name])){
          $isDitto = true;
          foreach($rec_name_chks as $rec_name_chk){
            if(!$chk = $name_chks[$fld_name]){
              break;
            }

            if(!array_diff_assoc($rec_name_chk, $chk)){
              $isDitto = false;
              break;
            }
          }

          if($isDitto){
            switch ($method) {
              case 'copy':
                for($i = $ind - 1; $ind > 0; $ind--){
                  if(isset($recs[$i][$fld_name])){
                    $rec[$fld_name] = $recs[$i][$fld_name];
                    break;
                  }
                }
                break;

              case 'merge':
              case 'ditto':
                $rec[$fld_name] = '_DITTO_';
                break;
            }
          }
        }
      }
    }

    return $recs;
  }
}

/*
class OerpNode
{
  private $id;
  private $parent;
  private $children;

  function __construct($id, $Parent){
    $this->id = $id;
    $this->parent = $Parent;
    $this->children = array();
  }

  function view($indent = ''){
    $buf = $indent . (string)$this->id . "\n";

    if($this->children != array()){
      foreach($this->children as $child){
        $buf .= $child->view($indent . '-');
      }
    }

    return $buf;
  }

  function getId(){
    return $this->id;
  }

  function parent($NewChild = null){
    if($NewChild === null){
      return $this->parent;
    }
    else{
      $this->parent = $NewChild;
    }
  }

  function getChild($ind){
    return $this->children[$ind];
  }

  function addChild($NewChild){
    $this->children[] = $NewChild;
  }

  function getChildren(){
    return $this->children;
  }

  function countChildren(){
    return count($this->children);
  }
}

class OerpTree
{
  private $count;
  private $nodes;
  protected $storage;
  private $ptr;

  function __construct(){
    $this->nodes[0] = new OerpNode(0, null);
    $this->count = 1;
    $this->storage = array();
    $this->ptr = 0;
  }

  public function getNodes(){
    return $this->nodes;
  }

  public function getNode($id = null){
    if(is_null($id)){
      return $this->nodes[$this->ptr];
    }
    else{
      return $this->nodes[$id];
    }
  }

  protected function setNodes($nodes){
    $this->nodes = $nodes;
  }

  public function view(){
    return $this->nodes[0]->view();
  }

  public function root(){
    $this->ptr = 0;
    return $this;
  }

  public function append($item, $slot = '%val'){
    if($slot == '%attr' || $slot == '$node'){
      die('Slot "' . $slot . '" is reserved');
    }

    $this->nodes[$this->count] =
    new OerpNode($this->count, $this->nodes[$this->ptr]);

    $this->nodes[$this->ptr]->addChild($this->nodes[$this->count]);
    $this->storage[$this->count][$slot] = $item;
    $this->storage[$this->count]['%attr'] = array();
    $this->count++;
    return $this;
  }

  public function in(){
    $last = $this->nodes[count($this->nodes) - 1];
    $this->go($last);
    return $this;
  }

  public function go($target){
    if($target instanceof OerpNode){
      $this->ptr = $target->getId();
    }else{
      $this->ptr = $target;
    }
    return $this;
  }

  public function parent(){
    if($this->getId() === 0){
      return null;
    }
    $this->go($this->getNode()->parent()->getId());
    return $this;
  }

  public function attr($input = null, $val = null){
    $id = $this->getId();

    if(is_array($input)){
      $this->storage[$id]['%attr'] = $input;
      return $this;
    }

    if(is_null($val)){
      if(is_null($input)){
        return $this->storage[$id]['%attr'];
      }
      return $this->storage[$id]['%attr'][$input];
    }
    $this->storage[$id]['%attr'][$input] = $val;
    return $this;
  }

  protected function props($val = null){
    if(is_null($val)){
      $props = $this->attr('props');
      return (empty($props)) ? array() : $props;
    }
    else{
      $this->attr('props', $val);
      return $this;
    }
  }

  public function getItem($id = null, $slot = '%val'){
    if($id === null){
      return $this->storage[$this->getId()][$slot];
    }
    return $this->storage[$id][$slot];
  }

  public function setItem($input, $id = null, $slot='%val'){
    $id = (is_null($id)) ? $this->getId() : $id;
    $this->storage[$id][$slot] = $input;
    return $this;
  }

  public function getParent($slot = '%val'){
    $cur = $this->getId();
     
    if($cur == 0){
      return null;
    }

    $id = $this->parent()->getId();
    $this->go($cur);

    return $this->getItem($id, $slot);
  }

  public function getDepth(){
    $cur = $this->getId();
    for($c = 0; $this->getId() != 0; $this->parent(), $c++);
    $this->go($cur);
    return $c;
  }

  public function getStorage(){
    return $this->storage;
  }

  public function getId(){
    return $this->getNode()->getId();
  }

  public function firstChild(){
    $this->go($this->getNode()->getChild(0)->getId());
    return $this;
  }

  public function lastChild(){
    $last = $this->getNode()->countChildren() - 1;
    $this->go($this->getNode()->getChild($last)->getId());
    return $this;
  }
   
  public function hasChild(){
    return ($this->getNode()->countChildren() > 0);
  }

  public function getChildren($slot = '%val', $isDeep = false){
    //  TODO check for infinite loop
    $ThisNode = $this->getNode();
    $cur = $ThisNode->getId();
    $children = array();

    foreach ($ThisNode->getChildren() as $Child){
      if($slot == '%node'){
        $children[] = $Child;
      }else{
        $children[] = $this->storage[$Child->getId()][$slot];
      }

      if($isDeep){
        $this->go($Child);
        $children = array_merge($children, (array)$this->getChildren($slot, true));
      }
    }
    return new OerpArray($children);
  }

  public function copyNodes($TreeFrom){
    $nodes = serialize($TreeFrom->getNodes());
    $this->nodes = unserialize($nodes);
  }
}
*/

class OerpFields extends OerpQuery
{
  private $vals;

  function __construct($mod)
  {
    parent::__construct(
      'ir.model.fields', array(array('model', '=', $mod))
    );
  }

  public function getVals()
  {
    return $this->vals;
  }

  public function setVal($key, $val)
  {
    $fld = $this->getDef($key);

    switch ($fld['ttype']) {
      case 'boolean':
        $this->vals[$key] = (bool)$val;
        break;
      case 'many2one':
      case 'one2many':
      case 'many2many':
      default:
        $this->vals[$key] = $val;
        break;
    }
  }

  public function mergeVal($key, $val)
  {
    $fld = $this->getDef($key);

    switch ($fld['ttype']) {
      case 'one2many':
      case 'many2many':
        $this->vals[$key] = array_merge(
          $this->vals[$key], (array)$val);
        break;
    }
  }

  public function getModel()
  {
    $cri = $this->getCriteria();
    return $cri[0][2];
  }

  public function getFields()
  {
    return $this->raw()->bunchKey('name');
  }

  public function defaults()
  {
    $Qry = new OerpQuery($this->getModel(), null, $this->getFields());
    return $Qry->defaults();
  }

  static public function getDef($mod, $cri = null)
  {
    $q = new OerpFields($mod);
    $defs = $q->exec(array('fields_get'));

    if (is_string($cri)) {
      if ($defs[$cri]) {
        return new OerpArray($defs[$cri]);
      } else {
        return null;
      }
    }
    if (is_array($cri)) {
      $keys = array_fill_keys($cri, null);

      return new OerpArray(
        array_intersect_key($defs->getArrayCopy(), $keys)
      );
    }

    foreach ($defs as &$def) {
      if ($def['type'] == 'selection') {
        $opts = $def['selection'];
        $def['selection'] = array();

        foreach ($opts as $opt) {
          $def['selection'][$opt[0]] = $opt[1];
        }
      }
    }

    return $defs;
  }

  public function isRelType($name)
  {
    $def = $this->getDef($name);
    if (in_array($def['ttype'],
                 array('one2many', 'many2one', 'many2many'))
    ) {

      return true;
    } else {
      return false;
    }
  }

  public function getRels($mod)
  {
    $rels = array();
    $flds = (array)$this->getDef($this->getModel())->filter(
      'return $item["relation"] == "' . $mod . '";');

    foreach ($flds as $fld) {
      $rels[] = array(
        'mod' => $fld['relation'],
        'ttype' => $fld['ttype'],
        'key' => $fld['name'],
      );
    }

    return $rels;
  }

  public function getFormEles()
  {
    $flds = $this->getDef($this->getModel());
    $eles = array();

    foreach ($flds as $fld) {
      $eles[$fld['name']]['#title'] = $fld['field_description'];

      switch ($fld['ttype']) {
        case 'text':
          $eles[$fld['name']]['#type'] = 'textarea';
          break;
        case 'char':
          $eles[$fld['name']]['#type'] = 'textfield';
          break;
        case 'float':
          $eles[$fld['name']]['#type'] = 'textfield';
          break;
        case 'integer':
          $eles[$fld['name']]['#type'] = 'textfield';
          break;
        case 'boolean':
          $eles[$fld['name']]['#type'] = 'checkboxes';
          $eles[$fld['name']]['#options'] = array('1' => '');
          break;
        case 'many2one':
          $name_fld = (strpos($fld['relation'], 'x_') === 0) ? 'x_name' : 'name';
          $Qry = new OerpQuery($fld['relation'], null, array($name_fld));
          $opts = (array)$Qry->raw()->promote('id', true);
          $opts[0] = '-None-';
          ksort($opts);

          $eles[$fld['name']]['#type'] = 'select';
          $eles[$fld['name']]['#options'] = $opts;

          break;
        case 'many2many':
        case 'one2many':
          //        need reviews
          $name_fld = (strpos($fld['relation'], 'x_') === 0) ? 'x_name' : 'name';
          $Qry = new OerpQuery($fld['relation'], null, array($name_fld));
          $opts = $Qry->raw()->promote('id', true);

          $eles[$fld['name']]['#type'] = 'checkboxes';
          $eles[$fld['name']]['#options'] = $opts;
          break;
        default:
          continue;
      }

      if ($fld['required']) {
        $eles[$fld['name']]['#required'] = true;
      }
    }
    return $eles;
  }

  static public function getCaption($fld, $arch)
  {
    $arch = new GuArchElement($arch);
    $eleFld = $arch->getArch();

    if ($eleFld->getAttribute('nolabel')) {
      $caption = '';
    }
    else {
      $caption = $eleFld->getAttribute('string');
      $caption = $caption ? $caption : $fld['string'];
      $caption = htmlspecialchars($caption);
      $caption = $fld['help'] ? '<sup>[?]</sup>' . $caption : '<sup> </sup>' . $caption;
    }

    return $caption;
  }

  static public function getEle($args)
  {
    $type = $args['field']['type'];
    $classname = 'Oerp\FormElement' . ucfirst($type);

    if(class_exists($classname)){
      $Ele = new $classname($args);
    }
    else{
      $Ele = new O\FormElementFallback($args);
    }
    return $Ele->getEntry();
  }

  static public function getSearchEle($fld, $arch, $default = null)
  {
    $args = array(
      'field' => $fld,
      'arch' => $arch,
    );

    switch ($fld['type']) {
      case 'char':
        $ele = self::getEle($args);
        $ele['#attributes']['op'] = 'ilike';

        break;

      case 'int':
        $ele = self::getEle($args);
        $ele['#attributes']['op'] = '=';
        break;

      case 'many2one':
        $ele = self::getEle($args);
        $ele['#type'] = 'textfield';
        $ele['#attributes']['op'] = 'ilike';
        unset($ele['#value']);
        break;

      case 'one2many':
        $ele['#type'] = 'textfield';
        $ele['#title'] = self::getCaption($fld, $arch);
        $ele['#description'] = $fld['help'];
        $ele['#attributes']['op'] = 'ilike';
        break;

      case 'float':
        $eleTpl = self::getEle($args);
        $eleTpl['#default_value'] = '0.0';
        unset($eleTpl['#required']);

        $ele['GE'] = $eleTpl;

        $ele['LE'] = $eleTpl;
        $ele['LE']['#title'] = '<sup> </sup>~';
        unset($ele['LE']['#description']);
        break;

      case 'date':
      case 'datetime':
        $eleTpl = self::getEle($args);
        $eleTpl['#default_value'] = '';
        $eleTpl['#attributes']['widget'] = 'datepicker';
          
        unset(
          $eleTpl['#required'],
          $eleTpl['#attributes']['readonly']
        );

        $ele['GE'] = $eleTpl;

        $ele['LE'] = $eleTpl;
        $ele['LE']['#title'] = '<sup> </sup>~';
        unset($ele['LE']['#description']);
        break;

      case 'boolean':
        $ele = array(
          '#type' => 'select',
          '#title' => self::getCaption($fld, $arch),
          '#description' => $fld['help'],
          '#attributes' => array('op' => '='),
          '#options' => array(
            'null' => '',
            'yes' => 'Yes',
            'no' => 'No'
          ),
          '#default_value' => 'null',
        );
        break;

      case 'selection':
        $ele = self::getEle($args);

        $ele['#options'] = array_merge(
          array('_null_' => '<None>'),
          $ele['#options']
        );

        $ele['#default_value'] = '_null_';
        $ele['#attributes']['op'] = '=';
        break;

      default:
        $ele = self::getEle($args);
    }

    unset(
    $ele['#required'],
    $ele['#attributes']['readonly'],
    $ele['#attributes']['disabled']
    );

    if (!is_null($default)) {
      switch ($fld['type']) {
        case 'char':
        case 'many2one':
        case 'one2many':
        $ele['#default_value'] = $default['ilike'];
          break;

        case 'int':
          $ele['#default_value'] = $default['='];
          break;

        case 'boolean':
          $ele['#default_value'] = $default['='] ? 'yes' : 'no';
          break;

        case 'float':
          if (isset($default['='])) {
            $default['>='] = $default['='];
            $default['<='] = $default['='];
          }
          $ele['GE']['#default_value'] = sprintf('%.1f', (float)$default['>=']);
          $ele['LE']['#default_value'] = sprintf('%.1f', (float)$default['<=']);
          break;

        case 'datetime':
          $ele['GE']['#default_value'] = $default['>='];
          $ele['LE']['#default_value'] = $default['<='];
          break;

        case 'selection':
          $ele['#default_value'] = $default['='];
          break;
      }
    }
    return $ele;
  }
}


class OerpObject extends OerpQuery
{
  function __construct($mod, $cri = array(), $flds = array(), $ids = null)
  {
    parent::__construct($mod, $cri, $flds, $ids);
  }

  public function defaults($GETNAME = false)
  {
    $q = new OerpQuery($this->getModel(), $this->getCriteria(), $this->getFields());
    $vals = $q->defaults();

    if ($GETNAME) {
      $defs = $this->getFieldsDef()->promote('name');

      foreach ($vals as $fname => &$val) {
        if ($val && in_array($defs[$fname]['ttype'],
                             array('one2many', 'many2one'))
        ) {

          $NameObj = new OerpObject(
            $defs[$fname]['relation'], null, array('name'), $val);

          $name = $NameObj->raw();
          $name = $name['name'];

          $val = $name;
        }
      }
    }
    return $vals;
  }

  public function getFields($pass = false)
  {
    $flds = parent::getFields();
    if (!empty($flds) || $pass) {
      return $flds;
    } else {
      return OerpFields::getDef($this->getModel())->bunchKey('name');
    }
  }

  public function getIds($PASS = false)
  {
    $ids = parent::getIds();

    if ($PASS) {
      return $ids;
    }

    if (empty($ids)) {
      $Tmp = new OerpQuery(
        parent::getModel(),
        parent::getCriteria(),
        array('id')
      );

      return $Tmp->raw()->bunchKey('id');
    } else {
      return $ids;
    }
  }

  public function getModelDef()
  {
    $mod = new OerpObject(
      'ir.model', array(array('model', '=', $this->getModel()))
    );

    $val = $mod->raw();
    return new OerpArray($val[0]);
  }

  public function getFieldsDef($FULL = false)
  {
    if (parent::getFields() == array() || $FULL == true) {
      return OerpFields::getDef($this->getModel());
    } else {
      return OerpFields::getDef($this->getModel(), $this->getFields());
    }
  }

  protected function getFieldDef($name)
  {
    return OerpFields::getDef($this->getModel(), $name);
  }

  public function refer($name, $flds = array(), $RTNOBJ = false)
  {
    $def = $this->getFieldDef($name);
    if (!in_array($def['ttype'],
                  array('many2one', 'one2many', 'many2many'))
    ) {

      return null;
    }

    $mem_ids = $this->collectMemIds($name);
    if (empty($mem_ids)) {
      return null;
    }

    $Target = new OerpObject(
      $def['relation'], null, $flds, $mem_ids);

    if ($RTNOBJ) {
      return $Target;
    } else {
      return $Target->raw();
    }
  }

  private function collectMemIds($name)
  {
    $val = $this->raw();
    $def = $this->getFieldDef($name);

    switch ($def['ttype']) {
      case 'many2one':
        $ids = $val->bunch($name);
        $ids = array_keys($ids);
        return $ids;
        break;
      case 'one2many':
        $ids = $val->collect($name);
        return $ids;
        break;
    }
  }

  public function union($NewSet)
  {
    if ($this->getModel() != $NewSet->getModel()) {
      die('Cannot get union from two differient models');
    }

    $ids = array_merge($this->getIds(), $NewSet->getIds());
    $flds = array_merge($this->getFields(), $NewSet->getFields());

    $Union = new OerpObject(
      $this->getModel(), null, $flds, $ids);

    return $Union;
  }

  public function intersect($Parent)
  {
    $Flds = new OerpFields($Parent->getModel());
    $rels = $Flds->getRels($this->getModel());
    $attr = $rels[0];
    //  TODO field to be selectable

    if (!empty($attr)) {
      if ($attr['mod'] == $this->getModel()) {
        $Tmp = new OerpQuery(
          $Parent->getModel(), $Parent->getCriteria(), array($attr['key']));

        if ($Parent->getIds() == array(0)) {
          return $attr;
        }

        $this_ids = $this->getIds(true);
        if (!empty($this_ids)) {
          $all_ids = $Tmp->raw()->bunchKey('id');
          $ids = array_intersect($this_ids, $all_ids);

          if (!empty($ids)) {
            $this->setIds($ids);
            return $attr;
          } else {
            return null;
          }
        }
        else {
          switch ($attr['ttype']) {
            case 'many2one':
              $this->setIds($Tmp->raw()->bunchKey($attr['key']));
              break;
            case 'one2many':
              $this->setIds($Tmp->raw()->collect($attr['key']));
              break;
          }
          return $attr;
        }
      }
    }
    return null;
  }
}

define('OERP_BTN_CHK', 1);
define('OERP_BTN_NEW', 2);
define('OERP_BTN_EDIT', 4);
define('OERP_BTN_DEL', 8);

class OerpButton
{
  private $btns;

  function __construct($btns)
  {
    $this->btns = $btns;
  }

  public function hasBtn($btn)
  {
    return $this->btns & $btn;
  }

  public function getHtml($btns = null)
  {
    $output = '';

    $chk = is_null($btns) ? $this->btns : $btns;

    if ($chk & OERP_BTN_CHK) {
      $output .= '
        <div class="oerp-button oerp-btn-chk ui-state-default ui-corner-all">
          <span class="oerp-icon ui-icon ui-icon-check"></span>
        </div>
      ';
    }

    if ($chk & OERP_BTN_EDIT) {
      $output .= '
        <div class="oerp-button oerp-btn-edit ui-state-default ui-corner-all">
          <span class="oerp-icon ui-icon ui-icon-pencil"></span>
        </div>
      ';
    }

    if ($chk & OERP_BTN_DEL) {
      $output .= '
        <div class="oerp-button oerp-btn-del ui-state-default ui-corner-all">
          <span class="oerp-icon ui-icon ui-icon-close"></span>
        </div>
      ';
    }

    return $output;
  }

  private function countBtn($EXC_NEW = false)
  {
    $bits = $this->btns;
    $c = 0;

    while ($bits) {
      $c += ($bits & 1) ? 1 : 0;
      $bits = $bits >> 1;
    }

    if ($EXC_NEW && $this->hasBtn(OERP_BTN_NEW)) {
      $c--;
    }

    return $c;
  }

  public function getWidthClass($EXC_NEW = false)
  {
    $c = $this->countBtn($EXC_NEW);
    $c = ($c < 1) ? 1 : $c;

    return 'oerp-op-' . $c;
  }
}

class OerpArchElement extends GuParamArch
{
  private $fields;

  private static $TYPE_MAP = array(
    'char' => 'textfield',
    'float' => 'textfield',
    'integer' => 'textfield',
    'boolean' => 'checkbox',
    'date' => 'textfield',
    'datetime' => 'textfield',
    'text' => 'textarea',
    'selection' => 'select',
    'many2one' => 'select',
    'one2many' => 'form',
  );

  function __construct($arch, $override = array())
  {
    parent::__construct($arch, $override);

    $params = $this->getParam();
    $params['oerp:model'] = $this->getInitModel($params, $override);
    $params['oerp:criteria'] = $this->getInitCriteria($params, $override);
    $params['oerp:fields'] = $this->getInitFields($params, $override);
    $params['oerp:ids'] = $this->getInitIds($params, $override);

    $this->getArch()->setAttribute('model', $params['oerp:model']);
    
    $this->param = $params;
    $this->mapEles();
  }

  protected function getInitModel($params, $override)
  {
    $model = $this->attr('model');
    $model = ($over = $this->attr('relation')) ? $over : $model;
    $model = ($over = $params['model']) ? $over : $model;
    $model = ($over = $params['oerp:model']) ? $over : $model;
    $model = ($over = $override['model']) ? $over : $model;
    $model = ($over = $override['oerp:model']) ? $over : $model;
    return $model;
  }

  protected function getInitCriteria($params, $override)
  {
    $cri = json_decode($params['oerp:criteria']);
    $cri = ($over = $this->getArch()->getAttribute('criteria'))
        ? $over : $cri;

    $cri = ($over = $override['criteria']) ? $over : $cri;
    $cri = ($over = $override['oerp:criteria']) ? $over : $cri;

    if ($cri_ex = OerpSearchPanelController::decodeCri($override['oerp:cri_ex'])) {
      foreach ($cri_ex as $each) {
        $cri[] = $each;
      }
    }
    return $cri;
  }

  protected function getInitFields($params, $override)
  {
    $flds = $this->getFields($this->getArch());
    $flds = ($over = $params['oerp:fields']) ? $over : $flds;
    $flds = ($over = json_decode($override['oerp:fields'])) ? $over : $flds;
    return $flds;
  }

  protected function getInitIds($params, $override)
  {
    $ids = $params['oerp:ids'];
    $ids = is_string($ids) ? json_decode($ids) : $ids;
    $ids = ($over = $this->getArch()->getAttribute('ids'))
        ? $over : $ids;

    $over = is_string($override['oerp:ids'])
        ? json_decode($override['oerp:ids'])
        : $override['oerp:ids'];

    $ids = $over ? $over : $ids;
    return $ids;
  }

  protected function getFieldEleList()
  {
    $xp = <<<XPATH
/form//field[not(self::*/ancestor::field[1])][not(self::*/ancestor::tree[1])]
| /form//tree
| /field/field
| /tree/field
XPATH;

    return $this->xp($xp, $this->getArch());
  }

  public function getFields()
  {
    $flds = array();

    foreach($this->getFieldEleList() as $eleFld){
      $flds[] = $eleFld->getAttribute('name');
    }

    return $flds;
  }

  protected function getFieldDef($mod = null)
  {
    if (!$mod) {
      return $this->fields;
    }
    else {
      if (!$this->fields[$mod]) {
        $this->fields[$mod] = (array)OerpFields::getDef($mod);

        return $this->fields[$mod];
      }
      else {
        return $this->fields[$mod];
      }
    }
  }

  protected function mapEle($fld)
  {
    if ($fld->getAttribute('OerpArchElement_mapped')) {
      return;
    }

    $name = $fld->getAttribute('name');
    $model = $fld->getAttribute('model');
    $fldDef = $this->getFieldDef($model);

    switch ($fld->nodeName) {
      case 'form':
        foreach ($this->xp('.//field | ./tree', $fld) as $inn_fld) {
          if ($fld->hasAttribute('relation')) {
            $inn_fld->setAttribute('model', $fld->getAttribute('relation'));
          }
          else {
            $inn_fld->setAttribute('model', $model);
          }
        }
        break;

      case 'tree':
        if(!$fldDef[$name]['relation']){
          $fld->setAttribute('relation', $fld->getAttribute('model'));
        }
        else{
          $fld->setAttribute('relation', $fldDef[$name]['relation']);
        }

        $relation = $fld->getAttribute('relation');

        if($fldDef[$name]['type']){
          $fld->setAttribute('ttype', $fldDef[$name]['type']);
        }

        $xp = './/field[not(self::*/ancestor::field[1])]';

        foreach ($this->xp($xp, $fld) as $inn_fld) {
          $inn_fld->setAttribute('model', $relation);
        }
        break;

      case 'field':
        if (!$name)
          return;

        $fld->setAttribute('ttype', $fldDef[$name]['type']);

        if ($fldDef[$name]['relation'] && $fldDef[$name]['relation'] != 'NULL') {
          $fld->setAttribute('relation', $fldDef[$name]['relation']);
        }

        if (!$fld->getAttribute('string')) {
          $fld->setAttribute('string',
                             ($fldDef[$name]) ? $fldDef[$name]['field_description'] : $name);
        }

        if ($fld->hasChildNodes()) {
          foreach ($this->xp('.//field', $fld) as $inn_fld) {
            if ($fld->hasAttribute('relation')) {
              $inn_fld->setAttribute('model', $fld->getAttribute('relation'));
            }
            else {
              $inn_fld->setAttribute('model', $model);
            }

            if ($anchor = $inn_fld->getAttribute('anchor')) {
              $inn_fld->setAttribute('anchor', $anchor . '.' . $name);
            }
            else {
              $inn_fld->setAttribute('anchor', $name);
            }
          }
        }
        break;
    }

    $fld->setAttribute('OerpArchElement_mapped', '1');
  }

  protected function mapEles()
  {
    $this->walk(array($this, 'mapEle'));
  }

  protected function isWrapperField($eleFld)
  {
    return !!$this->xp('.//field[1]', $eleFld)->length;
  }
}

class OerpRecord extends OerpArchElement
{
  private $path;

  static protected $prefix = array(
    'select', 'view',
  );

  function __construct($ele, $path = '', $param = array())
  {
    parent::__construct($ele, $param);
    $this->path = $path;
  }

  public function getRecord($param = array())
  {
    $param = $this->getParam(null, $param);
    $q = new OerpNestQuery($this->dump($this->getArch()), $param);
    $rec = $q->sum(array('include_id'));
    return $rec[0];
  }

  protected function mapFormArch($arch = null, $val = null)
  {
    if (is_null($arch)) {
      $arch = $this->getArch();
    }

    if (is_null($val)) {
      $val = $this->getRecord();
    }

    foreach ($this->xp('./*', $arch) as $fld) {
      $name = $fld->getAttribute('name');

      if ($this->xp('.//field', $fld)->length) {
        switch ($fld->nodeName) {
          case 'field':
            $this->mapFormArch($fld, $val[$name]);
            break;

          case 'tree':
            $ids = json_decode($this->getParam('oerp:ids'));
            $name = $fld->getAttribute('name');

            $q = new OerpQuery(
              $this->getArch()->getAttribute('model'),
              array(array('id', '=', $ids[0])),
              array($name)
            );

            $resp = $q->raw();

            $paramIds = $fld->ownerDocument->createElement(
              'param', json_encode($resp[0][$name]));

            unset($resp);

            $paramIds->setAttribute('name', 'oerp:ids');
            $fld->appendChild($paramIds);

            break;

          default:
            $this->mapFormArch($fld, $val);
            break;
        }
      }
      else if ($fld->nodeName == 'field') {
        $eleNodeValue = $this->getDoc()->createElement('param');
        $eleNodeValue->setAttribute('name', 'default');

        $eleAttrs = $this->getDoc()->createElement('param');

        if (isset($val[$name]) && !is_array($val[$name])) {
          $eleNodeValue->nodeValue = $val[$name];
        }

        $fld->appendChild($eleNodeValue);
        $fld->appendChild($eleAttrs);
      }
    }
  }

  public function getFormEntry()
  {
    $this->mapFormArch();
    $f = new OerpArchForm($this->dump());
    $form = $f->getFormEntry();
    return $form;
  }

  private function isValueToSkip($name){
    return ($name == 'id') ? true : false;
  }

  private function isValueToRemove($name){
    if (strpos($name, 'func:') === 0)
      return true;

    if (in_array($name, array('form_build_id', 'form_token', 'form_id')))
      return true;

    // ignore not sending eles
    $toks = explode(':', $name);
    if ($this->getParam('action') == 'send' && in_array($toks[0], self::$prefix))
      return true;

    return false;
  }

  private function prepareSendValue($vals){
    if ($this->getParam('action') == 'send') {
      $model = $this->attr('relation')
          ? $this->attr('relation')
          : $this->attr('model');

      if($vals['id']){
        $id = $vals['id'];
        unset($vals['id']);

        switch ($this->attr('ttype')) {
          case 'one2many':
            return array(array(1, $id, $vals));

          case 'many2one':
            $q = new OerpQuery($model, null, null, $id);
            $q->write($vals);
            return $id;
        }
      }
      else{
        $q = new OerpFields($model);
        $vals = array_merge($q->defaults(), $vals);

        switch ($this->attr('ttype')) {
          case 'one2many':
            return array(array(0, 0, $vals));

          case 'many2one':
            $resp = $q->create($vals);
            return $resp;
        }
      }
    }
    return $vals;
  }

  private function getOne2manySendValue($val, $ele)
  {
    $model = ($model = $ele->getAttribute('relation'))
        ? $model : $ele->getAttribute('model');

    if($val['id']){
      $id = $val['id'];
      unset($val['id']);
      return array(1, $id, $val);
    }
    else{
      $q = new OerpFields($model);
      $val = array_merge($q->defaults(), $val);
      return array(0, 0, $val);
    }
  }

  private function sendMany2oneValue($val, $ele)
  {
    if(!is_array($val)){
      return $val;
    }

    $model = ($model = $ele->getAttribute('relation'))
        ? $model : $ele->getAttribute('model');

    if($val['id']){
      $id = $val['id'];
      unset($val['id']);

      $q = new OerpQuery($model, null, null, $id);
      $q->write($val);
      return $id;
    }
    else{
      $q = new OerpQuery($model);
      $val = array_merge($q->defaults(), $val);
      unset($val['id']);
      return $q->create($val);
    }
  }

  private function grabEle($name, $par_node = null)
  {
    if(is_null($par_node))
      $par_node = $this->getArch();

    if (!$this->path) {
      $rel = ($rel = $par_node->getAttribute('relation'))
          ? $rel : $par_node->getAttribute('model');

      $xpath = sprintf(
        '//*[@name="%s" and @model="%s"][1]',
        $name, $rel
      );

      return $this->xp($xpath)->item(0);
    }
    else {
      $xpath = '';
      $toks = explode('/', $this->path);
      $toks[] = $name;

      foreach ($toks as $tok) {
        $xpath .= sprintf('//*[@name="%s"]', $tok);
      }

      return $this->xp($xpath)->item(0);
    }
  }

  private function adaptFlatValue($ele, $val)
  {
    switch ($ele->getAttribute('ttype')) {
      case 'boolean':
        $val = (bool)$val;
        break;

      case 'integer':
        $val = (int)$val;
        break;

      case 'char':
      case 'text':
      case 'selection':
        $val = (string)$val;
        break;

      case 'float':
        $val = (float)$val;
        break;

      case 'many2one':
        if ($ele->hasAttribute('option_key')) {
          $opt = $this->xp('./option[@value="' . $val . '"]', $ele)->item(0);

          if (!$opt) {
            dpr($val);
            dpr($ele->getAttribute('name'));
            dpr($this->dump($ele));

            echo '<pre>';
            throw new Exception();
          }

          $val = (int)$opt->getAttribute('oerp_rid');
        }
        else {
          $val = (int)$val;
        }
        break;
    }
    return $val;
  }

  private function adaptNestedValue($ele, $val)
  {
    if ($ele->tagName == 'tree') {
      $type = $this->xp('./param[@name="type"]', $ele)
          ->item(0)->nodeValue;

      switch ($type) {
        case 'select-one':
          return $val;

        case 'select-many':
          $sels = array();
          foreach ($val as $sel_key => $sel) {
            if ($sel) {
              preg_match('/sel-(\d+)/', $sel_key, $m);
              $sels[] = $m[1];
            }
          }

          return $sels;
      }
    }

    if (!$ele) {
      $ele = $this->getArch();
    }

    if ($this->getParam('action') == 'send') {
      $param_act = $ele->ownerDocument->createElement('param', 'send');
      $param_act->setAttribute('name', 'action');
      $ele->appendChild($param_act);
    }

    $f = new OerpRecord(
      $ele,
      $this->path
          ? $this->path . '/' . $this->attr('name')
          : $this->attr('name')
    );

    return $f->adaptValues($val);
  }

  private function isFieldToRemove($ele, $val)
  {
    $ttype = $ele->getAttribute('ttype');

    switch($ttype){
      case 'many2one':
      case 'many2many':
      case 'one2many':
      return empty($val) ? true : false;

      case 'date':
      case 'datetime':
      return empty($val) ? true : false;
    }
    return false;
  }

  private function recurMergeArray()
  {
    if (func_num_args() < 2) {
      trigger_error(__FUNCTION__ . ' needs two or more array arguments', E_USER_WARNING);
      return;
    }
    $arrays = func_get_args();
    $merged = array();
    while ($arrays) {
      $array = array_shift($arrays);
      if (!is_array($array)) {
        trigger_error(__FUNCTION__ . ' encountered a non array argument', E_USER_WARNING);
        return;
      }
      if (!$array)
        continue;
      foreach ($array as $key => $value)
        if (is_string($key))
          if (is_array($value) && array_key_exists($key, $merged) && is_array($merged[$key]))
            $merged[$key] = $this->recurMergeArray($merged[$key], $value);
          else
            $merged[$key] = $value;
        else
          $merged[] = $value;
    }
    return $merged;
  }

  public function prepareSend($vals)
  {
    foreach ($vals as $name => &$val) {
      if($this->isValueToSkip($name))
        continue;

      if($this->isValueToRemove($name)){
        unset($vals[$name]);
        continue;
      }

      $ele = $this->grabEle($name);
      if(!$ele){
        throw new Exception('[' . $name . ']');
      }

      if($this->isFieldToRemove($ele, $val)){
        unset($vals[$name]);
        continue;
      }

      $val = is_array($val)
          ? $this->adaptNestedValue($ele, $val)
          : $this->adaptFlatValue($ele, $val);
    }

    return $this->prepareSendValue($vals);
  }

  public function adaptValues($vals)
  {
    foreach ($vals as $name => &$val) {
      if($this->isValueToSkip($name))
        continue;

      if($this->isValueToRemove($name)){
        unset($vals[$name]);
        continue;
      }

      $ele = $this->grabEle($name);
      if(!$ele){
        echo '<pre>';
        throw new Exception();
      }

      if($this->isFieldToRemove($ele, $val)){
        unset($vals[$name]);
        continue;
      }

      $ttype = $ele->getAttribute('ttype');

      if($ttype == 'one2many'){
        foreach($val as &$each){
          $each = $this->adaptNestedValue($ele, $each);
        }
      }
      else{
        $val = is_array($val)
            ? $this->adaptNestedValue($ele, $val)
            : $this->adaptFlatValue($ele, $val);
      }
    }
    return $vals;
  }

  private function recurSend($vals, $ele = null)
  {
    if(!is_array($vals))
      return $vals;
    
    if(is_null($ele))
      $ele = $this->getArch();

    foreach($vals as $name => &$val){
      if($this->isValueToSkip($name))
        continue;

      $inner_ele = $this->grabEle($name, $ele);

      if(!$inner_ele){
        $arch = '<pre>' . htmlspecialchars($this->dump($ele)) . '</pre>';
        throw new Exception('[' . $name . ']' . $arch);
      }

      $ttype = $inner_ele->getAttribute('ttype');

      if($this->isWrapperField($inner_ele)){
        if($ttype == 'one2many'){
          $newVal = array();
          foreach($val as $each){
            $newVal[] = $this->recurSend($each, $inner_ele);
          }
          $val = $newVal;
        }
        else{
          $val = $this->recurSend($val, $inner_ele);
        }
      }

      switch($ttype){
        case 'many2one':
          $val = $this->sendMany2oneValue($val, $inner_ele);
          break;

        case 'one2many':
          $newValue = array();
          foreach($val as $each){
            $newValue[] = $this->getOne2manySendValue($each, $inner_ele);
          }
          $val = $newValue;
          break;
      }
    }
    return $vals;
  }

  public function send($vals)
  {
    $vals = $this->adaptValues($vals);

    if($this->getParam('oerp:ids')){
      $vals = $this->recurMergeArray($this->getRecord(), $vals);
    }

    $vals = $this->recurSend($vals);

    if($vals['id']){
      unset($vals['id']);
      $q = new OerpArchQuery($this->dump(), $this->getParam());
    
      return $q->write($vals);
    }
    else{
      unset($vals['id']);
      $q = new OerpQuery($this->getArch()->getAttribute('model'));
      return $q->create($vals);
    }
  }
}

class OerpArchForm extends GuForm
{
  private $fields;
  private $defaults;

  private static $TYPE_MAP = array(
    'char' => 'textfield',
    'float' => 'textfield',
    'integer' => 'textfield',
    'boolean' => 'checkbox',
    'date' => 'textfield',
    'datetime' => 'textfield',
    'text' => 'textarea',
    'selection' => 'select',
    'many2one' => 'select',
    'one2many' => 'form',
  );

  function __construct($arch, $parent_model = null)
  {
    parent::__construct($arch);
    $this->mapEles();
  }

  protected function getFieldDef($mod = null)
  {
    if (!$mod) {
      return $this->fields;
    }
    else {
      if (!$this->fields[$mod]) {
        $this->fields[$mod] = (array)OerpFields::getDef($mod);

        return $this->fields[$mod];
      }
      else {
        return $this->fields[$mod];
      }
    }
  }

  private function defaults($mod = null, $fld = null)
  {
    if (is_null($mod) && is_null($fld)) {
      $models = array();
      foreach ($this->xp('//@model') as $ele) {
        $models[] = $ele->value;
      }

      $models = array_unique($models);

      foreach ($models as $model) {
        if ($model) {
          $q = new OerpFields($model);
          $this->defaults[$model] = $q->defaults();
        }
      }
      return $this->defaults;
    }

    if (!is_null($mod)) {
      if (!isset($this->defaults[$mod])) {
        $q = new OerpFields($mod);
        $this->defaults[$mod] = $q->defaults();
      }

      return is_null($fld)
          ? $this->defaults[$mod]
          : $this->defaults[$mod][$fld];
    }
    return null;
  }

  protected function getDefault($fld)
  {
    $default_val = $this->defaults(
      $fld->getAttribute('model'), $fld->getAttribute('name'));

    return $default_val;
  }

  protected function getEle($fld, &$form)
  {
    if ($fld->tagName == 'tree') {
      $t = new OerpTable($this->dump($fld));
      $form[$fld->getAttribute('name')] = $t->getFormEles();
    }
    else {
      parent::getEle($fld, $form);
    }
  }

  protected function mapEle($fld)
  {
    parent::mapEle($fld);

    if ($fld->getAttribute('mapped') >= 2) {
      return;
    }

    $model = $fld->getAttribute('model');

    switch ($fld->nodeName) {
      case 'tree':
        break;

      case 'form':
        foreach ($this->xp('.//field', $fld) as $inn_fld) {
          if ($this->xp('./ancestor::tree[1]', $inn_fld)->length) {
            continue;
          }

          if ($fld->hasAttribute('relation')) {
            $inn_fld->setAttribute('model', $fld->getAttribute('relation'));
          }
          else {
            $inn_fld->setAttribute('model', $model);
          }
        }
        break;

      case 'field':
        if ($this->xp('./ancestor::tree[1]', $fld)->length) {
          continue;
        }

        $fldDef = OerpFields::getDef($model);

        if (!$name = $fld->getAttribute('name')) {
          return;
        }

        if ($fldDef[$name]['relation'] && $fldDef[$name]['relation'] != 'NULL') {
          $fld->setAttribute('relation', $fldDef[$name]['relation']);
        }

        if (!$fld->getAttribute('string')) {
          $fld->setAttribute('string',
                             ($fldDef[$name]) ? $fldDef[$name]['string'] : $name);
        }

        if ($fld->hasChildNodes()) {
          foreach ($this->xp('.//field', $fld) as $inn_fld) {
            if ($fld->hasAttribute('relation')) {
              $inn_fld->setAttribute('model', $fld->getAttribute('relation'));
            }
            else {
              $inn_fld->setAttribute('model', $model);
            }
          }
        }

        $fld->setAttribute('ttype', $fldDef[$name]['type']);
        if (!$fld->getAttribute('type')) {
          if ($mapped_type = self::$TYPE_MAP[$fldDef[$name]['type']]) {
            $fld->setAttribute('type', $mapped_type);
          }
        }

        switch ($fldDef[$name]['type']) {
          case 'selection':
            break;

          case 'many2one':
            $doc = $fld->ownerDocument;
            $key = $fld->getAttribute('key');

            if($key) {
              $opt_key = $fld->getAttribute('option_key');
              $opt_key = $opt_key ? $opt_key : 'id';

              $q = new OerpQuery($fldDef[$name]['relation'], null,
                array_unique(array($key, $opt_key)));

              //            no-cache query
              $recs = (array)$q->raw(false, true);
              usort($recs, create_function('$a,$b', sprintf('
                return strcmp($a["%s"], $b["%s"]);
              ', $key, $key)));

              foreach ($recs as $rec) {
                $newOpt = $doc->createElement(
                  'option', htmlspecialchars($rec[$key]));

                $newOpt->setAttribute('value', $rec[$opt_key]);
                $newOpt->setAttribute('oerp_rid', $rec['id']);
                $fld->appendChild($newOpt);
              }
            }
            break;
        }
        break;
    }

    $fld->setAttribute('mapped', $fld->getAttribute('mapped') + 1);
  }

  public function getFormEles($node = null, &$form = null)
  {
    $form_send = parent::getFormEles($node, $form);
    $form_send['#attributes'] = array('class' => 'oerp-formview');
    return $form_send;
  }

  protected function getInnerForm($fld, &$form)
  {
    $doc = new DOMDocument();
    $elForm = $doc->createElement('form');
    $elForm->setAttribute('model', $fld->getAttribute('relation'));
    $elForm->setAttribute('mapped', 2);

    foreach ($fld->childNodes as $chd) {
      $elForm->appendChild($doc->importNode($chd, true));
    }
    $doc->appendChild($elForm);

    $inner_form = new OerpArchForm($doc);
    $form[$fld->getAttribute('name')] = $inner_form->getFormEles();
  }

  public function theme(&$form)
  {
    $theme = '';
    if ($title = $this->attr('string')) {
      $theme .= '<div class="caption title">' . $title . '</div>';
    }

    $theme .= parent::theme($form);

    $output = sprintf('
      <div class="oerp-view" oerp_vid="%s" oerp_model="%s">
        <div class="oerp-widget-content ui-widget-content">%s</div>
      </div>
    ', $this->attr('vid'), $this->attr('model'), $theme);

    jquery_ui_add(array('ui.tabs', 'ui.draggable', 'ui.dialog', 'ui.datepicker'));
    OerpUtil::addCss();
    OerpUtil::addJs('jquery.oerpview', 'formview');

    return $output;
  }

  public function send($vals)
  {
    $Rec = new OerpRecord($this->dump());
    $resp = $Rec->send($vals);

    if ($resp) {
      drupal_set_message('Order created successfully.');
    }
    else {
      drupal_set_message('Failed to create new order.', 'error');
    }
  }
}

class OerpCsvBuffer extends GuCsvBuffer
{
  protected function getFuncPrefix()
  {
    return array_merge(array('OerpFunc_'), parent::getFuncPrefix());
  }
}

class OerpJsHelper
{
  static public function flatJsSend($nested, $id = null)
  {
    if (!is_array($nested) && !is_object($nested)) {
      return null;
    }

    $nested = (array)$nested;

    //render records
    if ($id === (string)(int)$id || strpos($id, '0-') === 0) {
      //  records to delete
      if (isset($nested['#delete'])) {
        return ($id == 0)
            ? null
            : array(2, (int)$id, false);
      }

      return ($id == 0)
          ? array(0, 0, self::flatJsSend($nested['#value']))
          : array(1, (int)$id, self::flatJsSend($nested['#value']));
    }

    //render single-value fields
    if (isset($nested['#value'])) {
      switch ($nested['#type']) {
        case 'boolean':
          return (bool)$nested['#value'];
        case 'integer':
        case 'many2one':
          return (int)$nested['#value'];

        case 'float':
          return (float)$nested['#value'];

        case 'many2many':
          return null;

        case 'date':
          return false;

        default:
          return $nested['#value'];
      }
    }

    //render multi-value fields
    $vals = array();
    foreach ($nested as $key => $val) {
      if ($key === (string)(int)$key || strpos($key, '0-') === 0) {
        $resp = self::flatJsSend($val, $key);

        if (!is_null($resp)) {
          $vals[] = $resp;
        }
      }
      else {
        if (strpos($key, 'view[') === 0) {
          preg_match('/view\[(.*)\]/', $key, $m);
          $key = $m[1];
        }

        $vals[$key] = self::flatJsSend($val, $key);
      }
    }
    return $vals;
  }
}

class OerpUtil
{
  static public function jsondecode($data)
  {
    $data = preg_replace("/%u([0-9a-f]{3,4})/i", "&#x\\1;", urldecode($data));
    $data = html_entity_decode($data, null, 'UTF-8');
    ;
    $data = json_decode($data);
    return $data;
  }

  static public function renderArrayInput($input)
  {
    $vals = array();
    for ($c = 0; $c < count($input); $c++) {
      foreach ($input[$c] as $key => $val) {
        $tok = explode('.', $key);

        if (count($tok) > 1) {
          $vals[$c][$tok[0]][$tok[1]] = trim($val);
        }
        else {
          if ($key == 'name') {
            $val = implode(' ', array_map('trim', explode("\n", $val)));
          }
          $vals[$c][$tok[0]] = trim($val);
        }
      }
    }

    return $vals;
  }

  static public function renderNestedInput($nested, $id = null)
  {
    if (!is_array($nested) && !is_object($nested)) {
      return null;
    }

    $nested = (array)$nested;

    //render records
    if ($id === (string)(int)$id || strpos($id, '0-') === 0) {
      //  records to delete
      if (isset($nested['#delete'])) {
        return ($id == 0)
            ? null
            : array(2, (int)$id, false);
      }

      return ($id == 0)
          ? array(0, 0, self::renderNestedInput($nested['#value']))
          : array(1, (int)$id, self::renderNestedInput($nested['#value']));
    }

    //render single-value fields
    if (isset($nested['#value'])) {
      $args = array(
        'field' => array('type' => $nested['#type']),
        'value' => $nested['#value'],
      );
      return O\FormElementFactory::prepareSend($args);
    }

    //render multi-value fields
    $vals = array();
    foreach ($nested as $key => $val) {
      if ($key === (string)(int)$key || strpos($key, '0-') === 0) {
        $resp = self::renderNestedInput($val, $key);

        if (!is_null($resp)) {
          $vals[] = $resp;
        }
      }
      else {
        if (strpos($key, 'view[') === 0) {
          preg_match('/view\[(.*)\]/', $key, $m);
          $key = $m[1];
        }

        $vals[$key] = self::renderNestedInput($val, $key);
      }
    }
    return $vals;
  }

  static public function addCss()
  {
    drupal_add_css(drupal_get_path('module', 'oerp') . '/oerp.css');
  }

  static public function addJs()
  {
    drupal_add_js(drupal_get_path('module', 'oerp') . '/oerp.js');

    foreach (func_get_args() as $file) {
      drupal_add_js(drupal_get_path('module', 'oerp') . '/' . $file . '.js');
    }
  }
}

class OerpHookController
{
  public function getXML($name)
  {
    $sql = 'SELECT conf FROM {oerp_hook} WHERE name="%s"';
    return db_result(db_query($sql, $name));
  }

  public function getArray($name)
  {
    $xml = $this->getXML($name);

    if(!$xml){
      return array();
    }

    $Arch = new GuArchParam($xml);
    $ref = $Arch->getReference();

    $conf = array();
    foreach($ref as $fld_name => $val){
      $toks = explode('\\', $fld_name);
      $ptr = &$conf[array_shift($toks)];

      foreach($toks as $tok){
        unset($tmp);
        $tmp = &$ptr[$tok];
        unset($ptr);
        $ptr = &$tmp;
      }
      $ptr = $val['_param'];
    }
    return $conf;
  }
}