<?php
namespace Oerp\Modifier\Command;

class replaceDictKey extends \Gulei\Command\WithReference\_Abstract
{
  private $dict = array();

  public function execute($refVar = null)
  {
    $rows = &$refVar->refVar('rows');
    $dict_keys = array();

    foreach($this->ref->val() as $fld_name => $val){
      if(strpos($fld_name, '_') === 0){
        continue;
      }

      if($dict_key = $val['_attr']['dict_key']){
        $dict_keys[$val['_attr']['from']] = array(
          'model' => $val['_attr']['relation'],
          'key' => $dict_key,
          'fallback' => $val['_attr']['fallback'],
          'create_mask' => ($mask = $val['_attr']['create_mask']) ? (array)json_decode($mask) : array()
        );
      }
    }

    foreach($dict_keys as $from => $param){
      $domain = \Gulei\_Array::lambda($rows, 'collect', $from);

      $q = new \Oerp\Query\Basic(
        $param['model'],
        array(array($param['key'], 'in', $domain)),
        array('id', $param['key']
        )
      );

      $this->dict[$param['model']] = \Gulei\_Array::lambda($q->raw(), 'promote', $param['key'], true);
    }

    foreach($rows as &$row){
      foreach($dict_keys as $fld_name => $dict_key){
        $val = $row[$fld_name];
        $id = $this->dict[$dict_key['model']][$val];

        if(!$id){
          $fallback = $dict_key['fallback'];

          if($fallback == 'create'){
            $send = array_merge($dict_key['create_mask'], array($dict_key['key'] => $val));
            $op = new \Oerp\Query\Operation(
              'create', $dict_key['model'], null, null, $send);

            $id = $op->run();
          }
          else if($fallback == 'skip'){
            continue;
          }
          else{
            throw new \Exception('Dictionary key not found : ' . print_r($dict_key, true));
          }
        }
        $row[$fld_name] = $id;
      }
    }
  }
}