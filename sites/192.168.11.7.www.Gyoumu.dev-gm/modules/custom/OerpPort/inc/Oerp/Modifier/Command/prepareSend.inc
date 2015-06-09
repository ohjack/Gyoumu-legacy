<?php
namespace Oerp\Modifier\Command;

class prepareSend extends \Gulei\Command\WithReference\_Abstract
{
  private $remote_recs = array();
  private $flds_to_flush = array();
  private $RefVar;

  private function getFldsToFlush()
  {
    if($this->flds_to_flush){
      return $this->flds_to_flush;
    }

    $Arch = new \Gulei\Arch\Xml($this->ref->val('_arch'));

    foreach($Arch->xp('/tree/field[@act="write"][@ttype="one2many"]') as $fld){
      $fld_name = $fld->getAttribute('name');
      $this->flds_to_flush[$fld_name] = array(
        'model' => $fld->getAttribute('relation'),
        'name' => $fld_name,
      );
    }
    return $this->flds_to_flush;
  }

  private function getRemoteRecs()
  {
    if($this->remote_recs){
      return $this->remote_recs;
    }

    $attr = $this->ref->val('_attr');
    $from = $this->ref->val($attr['dict_key'], '_attr', 'from');

    $rows = $this->RefVar->refVar('rows');
    $context = \Gulei\_Array::lambda($rows, 'collect', $from);
    $flds = array_merge(array($attr['dict_key']), array_keys($this->getFldsToFlush()));

    $q = new \Oerp\Query\Basic(
      $attr['model'],
      array(array($attr['dict_key'], 'in', $context)),
      $flds
    );

    $this->remote_recs = \Gulei\_Array::lambda($q->raw(false, true), 'promote', $attr['dict_key']);
    return $this->remote_recs;
  }

  private function getFlushOps()
  {
    $ops = array();
    foreach($this->getFldsToFlush() as $fld){
      $ids = \Gulei\_Array::lambda($this->getRemoteRecs(), 'collect', $fld['name']);

      if($ids){
        $ops[] = new \Oerp\Query\Operation('unlink', $fld['model'], null, $ids, null);
      }
    }
    return $ops;
  }

  private function getUpdateOps()
  {
    $Mapper = new \Oerp\Mapper\Import(
      $this->RefVar->refVar('rows'));

    $sends = $Mapper->exe($this->ref->val('_arch'));
    $remote_recs = $this->getRemoteRecs();

    $attr = $this->ref->val('_attr');
    $ops = array();

    foreach($sends as &$send){
      $dict_key = $attr['dict_key'];
      $id = $remote_recs[$send[$dict_key]]['id'];
      $act = $attr['act'];

      if(!$id){
        $id = null;
        $act = 'create';
      }
      $ops[] = new \Oerp\Query\Operation($act, $attr['model'], null, $id, $send);
    }
    return $ops;
  }

  private function linkReference()
  {
    $Cmds = array(
      new \Oerp\Modifier\Command\replaceDictKey($this->ref),
    );

    foreach($Cmds as $Cmd){
      $Cmd->execute($this->RefVar);
    }
  }

  public function execute($RefVar = null)
  {
    $this->RefVar = $RefVar;
    $this->linkReference();

    $ops = array();
    $ops = array_merge($ops, $this->getFlushOps());
    $ops = array_merge($ops, $this->getUpdateOps());

    $rows = &$this->RefVar->refVar('rows');
    $rows = $ops;
  }
}