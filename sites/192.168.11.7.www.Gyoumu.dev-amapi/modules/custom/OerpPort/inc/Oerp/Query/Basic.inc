<?php
namespace Oerp\Query;
use Oerp as O;

class Basic
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
    self::$Client = new O\Query\Client($srv, $db, $uname, $pass);
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

    return new \Oerp\Util\_Array($resp);
  }

  public function raw($meta = false, $NOCACHE = false)
  {
    $val = php_xmlrpc_decode($this->rpc(false, $NOCACHE));

    if ($meta) {
      foreach ($val as &$each) {
        $each['%model'] = $this->model;
      }
    }

    $val = new \Oerp\Util\_Array($val);
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
