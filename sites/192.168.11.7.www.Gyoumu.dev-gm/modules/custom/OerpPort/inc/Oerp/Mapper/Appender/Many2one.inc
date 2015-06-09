<?php
namespace Oerp\Mapper\Appender;

class Many2one extends \Oerp\Mapper\Appender\_Abstract
{
  public function isApplicable()
  {
    $fld = $this->ref->val('field');
    return $fld->getAttribute('ttype') == 'many2one';
  }

  private function replaceVar(&$holder)
  {
    $rec = $this->ref->val('rec');

    if(preg_match('/^\$\{(.*)\}$/', $holder, $m)){
      if(!isset($rec[$m[1]])){
        throw new \Exception('Field not found');
      }
      $holder = $rec[$m[1]];
    }
  }

  protected function adapt()
  {
    list($val, $field) = $this->ref->vals('val', 'field');

    if($cri_src = $field->getAttribute('cri')){
      $cri = json_decode($cri_src);

      foreach($cri as &$cro){
        if(is_array($cro[2])){
          foreach($cro[2] as &$each){
            $this->replaceVar($each);
          }
        }
        else{
          $this->replaceVar($cro[2]);
        }
      }

      $q = new \Oerp\Query\Basic(
        $field->getAttribute('relation'), $cri, array('id'));

      $resp = $q->query(array('limit' => 1, 'offset' => 0), true);
      $id = $resp[0]['id'];

      if($id){
        return $id;
      }

      $fallback = $field->getAttribute('fallback');

      if($fallback == 'create'){
        $create_mask = (array)json_decode($field->getAttribute('create_mask'));
        return $q->create($create_mask);
      }
      else{
        throw new \Exception('No many2one reference found.');
      }
    }
    return $val;
  }
}