<?php
namespace Yauc\Table\Listings;

class _Abstract extends \Gulei\Table\Select
{
  protected $sellerId;

  protected  $select_sql;
  protected  $where_sql;
  protected  $record_sql;
  protected  $count_sql;

  function __construct($sellerId, $arch, $param = array()){
    $this->sellerId = $sellerId;
    $y = new \GuYapi($sellerId);
    $y->checkAccessToken();
    parent::__construct($arch, null, $param);
  }

  protected function getFuncPrefix()
  {
    return array_merge(array('yaucapiFunc_'), parent::getFuncPrefix());
  }

  public function getRecordsCount(){
    $rs = db_query($this->count_sql);
    return db_result($rs);
  }

  public function getDefaultParam(){
    $param = parent::getDefaultParam();
    $param['mark_translated'] = false;
    $param['pager'] = true;
    $param['items_per_page'] = 80;
    return $param;
  }

  protected function checkFuncName($src, $prefix = 'yaucapiFunc_'){
    $func_name = parent::checkFuncName($src, $prefix);

    if($func_name === false){
      return parent::checkFuncName($src);
    }
    else{
      return $func_name;
    }
  }

  protected function getSelectionValue($rec, $ind){
    return $rec['auc_id'];
  }

  protected function getStatus($auc_id){
    $CtrlFlag = new \YaucFlagController($auc_id);
    $flags = $CtrlFlag->getAucFlags();

    if($flags->to_end && $flags->to_kill == YAPI_FLAG_TOKILL){
      return 'Ending & To Kill';
    }

    if($flags->to_end){
      return 'Ending';
    }

    if($flags->to_kill == YAPI_FLAG_TOKILL){
      return 'To Kill';
    }

    if($flags->to_kill == YAPI_FLAG_KILLING
       || $flags->to_kill == YAPI_FLAG_KILLING_SCHEDULED ){
      return 'Killing';
    }
  }
}