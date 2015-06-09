<?php 

define('OERP_WRITE_CREATE', 0);
define('OERP_WRITE_EDIT', 1);
define('OERP_WRITE_DEL', 2);

function _erp_connect($srv = null, $db = null, $usr = null, $pwd = null)
{
  $pack = array();
  $pack['srv'] = is_null($srv) ? variable_get('oerpport_server', '') : $srv;
  $pack['db'] = is_null($db) ? variable_get('oerpport_db', '') : $db;
  $pack['usr'] = is_null($usr) ? variable_get('oerpport_uname', '') : $usr;
  $pack['pwd'] = is_null($pwd) ? variable_get('oerpport_pass', '') : $pwd;

  $tmp = new OerpObject('');
  $tmp->connect(
    $pack['srv'], $pack['db'], $pack['usr'], $pack['pwd']
  );
  unset($tmp);
}

class OerpClient
{
  private $server;
  private $database;
  private $username;
  private $password;
  private $uid = null;

  function __construct($srv = null, $db = null, $uname = null, $pass = null)
  {
    gulib_inc('xmlrpc.inc', 'lib');

    if ($srv && $db && $uname && $pass) {
      $this->connect($srv, $db, $uname, $pass);
    }
  }

  public function connect($srv, $db, $uname, $pass)
  {
    $this->server = $srv;
    $this->database = $db;
    $this->username = $uname;
    $this->password = $pass;

    $sock = new xmlrpc_client($this->server . '/xmlrpc/common');
    $msg = new xmlrpcmsg('login');
    $msg->addParam(new xmlrpcval($this->database));
    $msg->addParam(new xmlrpcval($this->username));
    $msg->addParam(new xmlrpcval($this->password));
    $resp = $sock->send($msg);

    if ($resp->faultCode()) {
      $this->printError($resp->faultString());
    }
    else {
      $this->uid = $resp->value()->scalarVal();

      if (!$this->uid) {
        die(sprintf(
          "Bad login or password from '%s' using database '%s'",
          $this->username, $this->database));
      }
    }
  }

  public function execute($args, $action = 'execute', $reset = false, $service = 'object')
  {
    if ($this->uid === null) {
      die('Error: Connection have not established');
      return null;
    }

    //encoding settings for reading-related actions
    global $xmlrpc_internalencoding;

    if ($service == 'object') {
      $pat = '/(create|write|search)/';
      if (preg_match($pat, $args[1])) {
        $xmlrpc_internalencoding = 'ISO-8859-1';
      } else {
        $xmlrpc_internalencoding = 'UTF-8';
      }

      //pad empty arguments
      switch ($args[1]) {
        case 'search':
          $args = array_pad($args, 4, array());
          break;
      }
    }

    $msg = new xmlrpcmsg($action);
    $msg->addParam(new xmlrpcval($this->database));
    $msg->addParam(new xmlrpcval($this->uid, "int"));
    $msg->addParam(new xmlrpcval($this->password));

    foreach ($args as $arg) {
      $msg->addParam(php_xmlrpc_encode($arg));
    }

    $debug = false;
    if ($debug) {
      _gecho($msg);
    }

    //  only cache search & read
    if ($args[1] != 'search' && $args[1] != 'read') {
      $reset = true;
    }

    if ($reset) {
      return $this->send($msg, $service);
    }

    //  cache using Drupal native API
    $cache_id = 'query:' . md5($msg->serialize());
    $cache = cache_get($cache_id, 'cache_oerpport');
    $time = $_SERVER['REQUEST_TIME'];

    if ($cache && $cache->expire > $time) {
      $resp = $cache->data;
    }
    else{
      $resp = $this->send($msg, $service);
      $expire = $this->getExpireTime($args);

      if ($expire) {
        cache_set($cache_id, $resp, 'cache_oerpport', $expire);
      }
    }
    
    return $resp;
  }

  protected function send($msg, $service)
  {
    $sock = new xmlrpc_client($this->server . '/xmlrpc/' . $service);
    $resp = $sock->send($msg);

    if ($resp->faultCode()) {
      $this->printError($resp->faultString());
    }

    return $resp->value();
  }

  public function exe($args)
  {
    return php_xmlrpc_decode($this->execute($args));
  }

  protected function getExpireTime($args)
  {
    $time = $_SERVER['REQUEST_TIME'];

    switch ($args[0]) {
      case 'ir.ui.view':
      case 'ir.model.fields':
//      expire in 1 day  
        return $time + 3600 * 24;

      default:
        return $time + 1;
    }
  }

  protected function printError($msg)
  {
    echo '<pre>' . $msg;
    throw new Exception('Error in OerpClient');
  }
}

class OerpQuery
{
  protected static $Client = null;
  private $model;
  private $criteria;
  private $fields;
  private $ids;

  function __construct($mod, $cri = array(), $flds = array(), $ids = null)
  {
    $this->model = $mod;
    $this->criteria = (empty($cri)) ? array() : $cri;
    $this->fields = (empty($flds)) ? array() : $flds;

    if ($ids instanceof xmlrpcval) {
      $this->ids = php_xmlrpc_decode($ids);
    }
    else if (empty($ids)) {
      $this->ids = array();
    }
    else if (is_array($ids)) {
      $this->ids = $ids;
    }
    else {
      $this->ids = array($ids);
    }
  }

  public function connect($srv = null, $db = null, $uname = null, $pass = null)
  {
    self::$Client = new OerpClient($srv, $db, $uname, $pass);
  }

  public function fields_view_get($id, $type)
  {
    $resp = self::$Client->execute(array(
                                        $this->model, 'fields_view_get', $id, $type, array()));

    return php_xmlrpc_decode($resp);
  }

  static public function _execute($args, $action = 'execute', $service = 'object')
  {
    $resp = self::$Client->execute(
      $args,
      $action,
      true,
      $service
    );

    return php_xmlrpc_decode($resp);
  }

  public function rpc($ID_ONLY = false, $NOCACHE = false)
  {
    $ids = php_xmlrpc_decode(
      self::$Client->execute(
        array($this->model, 'search', $this->criteria),
        'execute',
        $NOCACHE
      ));

    if ($this->ids) {
      $ids = array_values(array_intersect($ids, $this->ids));
    }

    if ($ID_ONLY) {
      return $ids;
    }

    $resp = self::$Client->execute(
      array($this->model, 'read', $ids, $this->fields),
      'execute',
      $NOCACHE
    );

    return $resp;
  }

  private function adaptPagerParam($param)
  {
    $paramPager = array();

    if($param['limit']){
      $paramPager['offset'] = (float)$param['offset'];
      $paramPager['limit'] = (float)$param['limit'];
    }
    else{
      $paramPager['offset'] = false;
      $paramPager['limit'] = false;
    }

    return $paramPager;
  }

  private function adaptCritera($cri)
  {
    if(is_array($cri))
      return $cri;

    if(is_string($cri))
      return json_decode($cri);

    throw new Exception('OerpQuery: Invalid criteria' . print_r($cri, true));
  }

  public function query($param = array(), $NOCACHE = false)
  {
    if ($this->ids) {
      $ids = $this->ids;
    }
    else {
      $paramPager = $this->adaptPagerParam($param);

      $resp = self::$Client->execute(
        array(
             $this->model,
             'search',
             $this->adaptCritera($this->criteria),
             $paramPager['offset'],
             $paramPager['limit']
        ),
        'execute',
        $NOCACHE
      );
      
      $ids = php_xmlrpc_decode($resp);
    }

    if ($param['id_only']) {
      return $ids;
    }

    $resp = self::$Client->execute(
      array($this->model, 'read', $ids, $this->fields),
      'execute',
      $NOCACHE
    );

    $resp = php_xmlrpc_decode($resp);

    if ($param['meta']) {
      foreach ($resp as &$each) {
        $each['%model'] = $this->model;
      }
    }

    return new OerpArray($resp);
  }

  public function raw($meta = false, $NOCACHE = false)
  {
    $val = php_xmlrpc_decode($this->rpc(false, $NOCACHE));

    if ($meta) {
      foreach ($val as &$each) {
        $each['%model'] = $this->model;
      }
    }

    $val = new OerpArray($val);
    //    $ids = $this->getIds();

    //    if(!empty($ids)){
    //      $val = $val->filter('
    //        return in_array($item["id"],' . var_export($ids, true) . ');');
    //    }
    return $val;
  }

  public function xml()
  {
    return $this->rpc()->serialize();
  }

  public function write($data)
  {
    $ids = $this->getIds();
    $ids = array_map('intval', $ids);

    if ($ids !== array(0)) {
      $resp = self::$Client->execute(
        array($this->getModel(), 'write', $ids, $data));
      //      $resp = $this->execute('write', $data);

      return php_xmlrpc_decode($resp);
    } else {
      die('Missing id to write record in "' . $this->getModel() . '"');
    }
  }

  public function create($data)
  {
    $resp = self::$Client->execute(
      array($this->getModel(), 'create', $data));

    return php_xmlrpc_decode($resp);
  }

  public function unlink()
  {
    $resp = self::$Client->execute(
      array($this->getModel(), 'unlink', $this->getIds())
    );

    return php_xmlrpc_decode($resp);
  }

  public function execute($act, $data)
  {
    $resp = self::$Client->execute(
      array($this->getModel(), $act, $data)
    );

    return php_xmlrpc_decode($resp);
  }

  public function exec($args, $NOCACHE = false)
  {
    $args = array_merge(array($this->getModel()), $args);
    $resp = self::$Client->execute(
      $args, 'execute', $NOCACHE
    );

    return php_xmlrpc_decode($resp);
  }

  public function exec_workflow($args)
  {
    $resp = self::$Client->execute(
      array_merge(array($this->model), $args),
      'exec_workflow'
    );

    return php_xmlrpc_decode($resp);
  }

  public function defaults($flds = array())
  {
    $fld_send = empty($flds) ? $this->fields : $flds;

    $resp = self::$Client->execute(array(
                                        $this->model, 'default_get', $fld_send));

    $vals = php_xmlrpc_decode($resp);
    return $vals;
  }

  public function default_get($flds)
  {
    return $this->defaults($flds);
  }

  public function name_get()
  {
    $resp = self::$Client->execute(array(
                                        $this->getModel(), 'name_get', $this->getIds()
                                   ));

    return php_xmlrpc_decode($resp);
  }

  public function getModel()
  {
    return $this->model;
  }

  public function getCriteria()
  {
    return $this->criteria;
  }

  public function getFields()
  {
    return $this->fields;
  }

  public function getIds()
  {
    return $this->ids;
  }

  public function setCriteria($cri)
  {
    if (is_array($cri)) {
      $this->criteria = $cri;
    }
    else {
      $this->criteria = json_decode($cri);
    }
  }

  public function setFields($flds)
  {
    $this->fields = $flds;
  }

  public function addField($fld)
  {
    if (!empty($this->fields)) {
      if (!in_array($fld, $this->fields)) {
        $this->fields[] = $fld;
      }
    }
  }

  public function setIds($ids)
  {
    if (is_array($ids)) {
      $this->ids = $ids;
    }
    else {
      $this->ids = array($ids);
    }
  }

  public function clearCache()
  {
    self::$Client->clearCache();
  }
}

class OerpQueryWizard extends OerpQuery
{
  static public function _execute($args, $action = 'execute')
  {
    return parent::_execute($args, $action, 'wizard');
  }
}

class OerpArchQuery extends OerpArchElement
{
  protected $q;
  protected $offset = 0;
  protected $limit = 0;

  private function initPagerParam($params, $override)
  {
    $items = $params['items_per_page'];
    $items = ($over = $override['items_per_page']) ? $over : $items;
    $page = $params['page'] ? $params['page'] : $_GET['page'];

    $this->limit = $items;
    $this->offset = $items * $page;
  }

  private function initFieldStrings()
  {
    $defs = OerpFields::getDef($this->getParam('oerp:model'));

    foreach ($this->xp('./field', $this->getArch()) as $field) {
      if (!$field->hasAttribute('string')) {
        $field->setAttribute(
          'string',
          $defs[$field->getAttribute('name')]['help']);
      }
    }
  }

  function __construct($arch_src, $override = array())
  {
    parent::__construct($arch_src, $override);
    $params = $this->getParam();

    $this->q = new OerpQuery(
      $params['oerp:model'],
      $params['oerp:criteria'],
      $params['oerp:fields'],
      $params['oerp:ids']
    );

    $this->initPagerParam($params, $override);
    $this->initFieldStrings();
  }

  public function raw()
  {
    return $this->q->raw();
  }

  public function getFields($arch = null)
  {
    $flds = array();

    foreach ($this->xp('./*', $arch) as $ele) {
      if ($ele->nodeName == 'field') {
        $name = $ele->getAttribute('name');

        if(strpos($name, 'func:') !== 0){
          $flds[] = $name;
        }
      }
      else if ($this->xp('.//field', $ele)) {
        $inner_flds = $this->getFields($ele);
        $flds = array_merge($flds, $inner_flds);
      }
    }

    return $flds;
  }

  public function getCount()
  {
    return count($this->q->query(array('id_only')));
  }

  public function refineQuery($args)
  {
    if ($args['ids']) {
      $this->q->setIds($args['ids']);
    }
  }

  public function write($vals)
  {
    return $this->q->write($vals);
  }
}

class OerpNestQuery extends OerpArchQuery
{
  private function getFlatValue($flags)
  {
    $flags = array_merge(
      array(
           'limit' => $this->limit,
           'offset' => $this->offset,
      ),
      $flags
    );

    return $this->q->query($flags);
  }

  private function getInheritFlags($flags)
  {
    $inherit_mask =
        array_fill_keys(array('meta', 'id_only', 'props'), null);

    return array_intersect_key($flags, $inherit_mask);
  }

  private function processWrapperField($val, $eleFld, $flags)
  {
    $inherit_flags = $this->getInheritFlags($flags);
    $ttype = $eleFld->getAttribute('ttype');
    $name = $eleFld->getAttribute('name');

    $ids = array();
    for ($c = 0; $c < count($val); $c++) {
      switch ($ttype) {
        case 'one2many':
          $ids = array_merge($ids, $val[$c][$name]);
          break;

        case 'many2one':
          $ids[] = $val[$c][$name][0];
          break;
      }
    }

    $ids = array_filter($ids);
    $ids = array_values($ids);

    if ($ids) {
      $q = new OerpNestQuery($this->dump($eleFld), array('oerp:ids' => $ids));
      $ref = $q->sum($inherit_flags)->bunch('id', !!$flags['include_id']);
    }
    else {
      $ref = array();
    }

    switch ($ttype) {
      case 'one2many':
        $val->linkMany($name, $ref);
        break;
      case 'many2one':
        $val->link($name, $ref);
        break;
    }
  }

  private function processFlatField($val, $eleFld)
  {
    $name = $eleFld->getAttribute('name');
    $ttype = $eleFld->getAttribute('ttype');

    switch ($ttype) {
      case 'one2many':
      case 'many2many':
        foreach ($val as &$rec) {
          $rec[$name] = implode('<br />', $rec[$name]);
        }
        break;
      case 'many2one':
        foreach ($val as &$rec) {
          if (is_array($rec[$name])) {
            $rec[$name] = $rec[$name][1];
          }
        }
        break;
    }
  }

  private function prepareSelectionOptions($val)
  {
    $defs = OerpFields::getDef($this->q->getModel());

    foreach ($val as &$rec) {
      foreach ($rec as $name => &$cell) {
        if ($defs[$name]['type'] == 'selection') {
          $cell = $defs[$name]['selection'][$cell];
        }
      }
    }
  }

  public function sum($flags = array())
  {
    $val = $this->getFlatValue($flags);

    if (!count($val)) {
      return new OerpArray();
    }

    foreach ($this->getFieldEleList() as $eleFld) {
      if ($this->isWrapperField($eleFld)){
        $this->processWrapperField($val, $eleFld, $flags);
      }
      else {
        $this->processFlatField($val, $eleFld);
      }
    }

    $this->prepareSelectionOptions($val);
    return $val;
  }

  public function getCount()
  {
    return count($this->getFlatValue(array('id_only', 'limit' => false)));
  }
}

function oerp_execute_js()
{
  $post = $_POST;
  $send = $post['send'];
  $send = json_decode($send);

  if ($post['service'] == 'wizard') {
    $value = (array)$send->value;
    $value = OerpJsHelper::flatJsSend($value['#value']);
    $datas = (array)json_decode($send->datas);

    $form = array_merge($datas, $value);

    $resp = OerpQueryWizard::_execute(
      array(
           (int)$send->wid,
           array(
             'model' => $send->model,
             'form' => $form,
             'id' => (int)$send->rid,
             'ids' => array(((int)$send->rid))
           ),
           $send->action
      )
    );
    return drupal_json($resp);
  }
  else if ($post['exec'] == 'exec_workflow') {
    $resp = OerpQuery::_execute(
      $send,
      'exec_workflow'
    );
    return drupal_json($resp);
  }
  else {
    switch ($send[1]) {
      case 'name_get':
        break;

      case 'write':
        $rec = (array)$send[3];

        if (isset($rec['#value'])) {
          $send[3] = OerpJsHelper::flatJsSend($rec['#value']);
        }
        break;

      case 'create':
        $rec = (array)$send[2];

        if (isset($rec['#value'])) {
          $send[2] = OerpJsHelper::flatJsSend($rec['#value']);
        }
        break;

      default:
        if ($post['onchange']){
          $resp = OerpQuery::_execute(
            $send
          );
          return drupal_json($resp);
        }
        else {
          $resp = OerpQuery::_execute(
            array(
                 $send[0],
                 $send[1],
                 array((int)$send[2])
            )
          );
        }
        return drupal_json($resp);
    }

    $q = new OerpQuery(array_shift($send));
    $resp = $q->exec($send, true);
    return drupal_json($resp);
  }
}

?>