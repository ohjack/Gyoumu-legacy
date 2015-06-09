<?php
namespace Gulei\Modifier\RecordProcessor;

abstract class _Abstract extends \Gulei\Modifier\_Abstract
{
  abstract protected function getType();

  protected function searchFunction($param)
  {
    $type = $this->getType();
    $ObjTable = $param['ObjTable'];

    if(!$ObjTable->getArch()){
      return array();
    }

    $eles = array();
    $eles[] = $ObjTable;

    if(isset($param[$type]) && $param[$type]){
      $eles[] = new \Gulei\Arch\Element('<root>' . $param[$type] . '</root>');
    }

    $funcs = array();
    foreach($eles as $ele){
      $funcNode = $ele->xp('./' . $type, $ele->getArch())->item(0);

      if($funcNode){
        foreach($ele->xp('./function | ./func', $funcNode) as $func_ele){
          $func = array();

          if($func_type = $func_ele->getAttribute('type')){
            $func['type'] = $func_type;
          }
          else{
            $func['type'] = 'error';
          }

          foreach($ele->xp('./source | ./src', $func_ele) as $node){
            $newNode = $ObjTable->getDoc()->importNode($node, true);
            $func['src'][] = $newNode;
          }

          $func['fld'] = array();
          foreach($ele->xp('./field | ./fld', $func_ele) as $node){
            $func['fld'][] = $node->nodeValue;
          }

          foreach($ele->xp('./message | ./msg', $func_ele) as $node){
            $func['msg'][] = trim($node->nodeValue);
          }

          $ref = array(
            'param' => $param,
            'func' => $func,
            'ObjTable' => $param['ObjTable'],
            'prefix' => $param['func_prefix'],
          );

          $classname = "Gulei\\Command\\Table\\" . ucfirst($type);
          $funcs[] = new $classname($ref);
        }
      }
    }
    return $funcs;
  }

  protected function modifyRows($RefVars, $ref)
  {
    $funcs = $this->searchFunction($ref->val('_param'));

    if(empty($funcs)){
      return;
    }

    $rows = &$RefVars->refVar('rows');

    foreach($funcs as $func){
      $refVar = new \Gulei\Manipulator\RefVariable();
      $refVar->addRefVar(array('recs' => &$rows));
      $func->execute($refVar);
    }
  }

  protected function isCellApplicable($RefVars, $ref)
  {
    return false;
  }

  protected function modify($RefVars, $ref)
  {
    return null;
  }
}