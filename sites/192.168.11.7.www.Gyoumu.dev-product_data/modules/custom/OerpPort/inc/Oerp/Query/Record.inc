<?php
namespace Oerp\Query;

class Record extends \Oerp\Arch\Element
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
    $q = new \Oerp\Query\Nested($this->dump($this->getArch()), $param);
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
            $ids = $this->getParam('oerp:ids');
            $ids = is_string($ids) ? json_decode($ids) : $ids;
            $name = $fld->getAttribute('name');

            $q = new \Oerp\Query\Basic(
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
    $f = new \Oerp\Arch\Form($this->dump());
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
            $q = new \Oerp\Query\Basic($model, null, null, $id);
            $q->write($vals);
            return $id;
        }
      }
      else{
        $q = new \Oerp\Query\Fields($model);
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
      $q = new \Oerp\Query\Fields($model);
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

      $q = new \Oerp\Query\Basic($model, null, null, $id);
      $q->write($val);
      return $id;
    }
    else{
      $q = new \Oerp\Query\Basic($model);
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
            throw new \Exception();
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

    $f = new \Oerp\Query\Record(
      clone $ele,
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
        throw new \Exception('[' . $name . ']');
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
        throw new \Exception();
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
        throw new \Exception('[' . $name . ']' . $arch);
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
      $q = new \Oerp\Query\Arch($this->dump(), $this->getParam());

      return $q->write($vals);
    }
    else{
      unset($vals['id']);
      $q = new \Oerp\Query\Basic($this->getArch()->getAttribute('model'));
      return $q->create($vals);
    }
  }
}