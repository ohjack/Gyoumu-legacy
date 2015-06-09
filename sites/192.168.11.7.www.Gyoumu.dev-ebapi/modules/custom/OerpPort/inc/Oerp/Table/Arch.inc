<?php
namespace Oerp\Table;

class Arch extends \Gulei\Table\_Abstract
{
  function __construct($arch, $param = array())
  {
    $param['arch'] = $arch;
    $param = array_merge(
      $this->getDefaultParam(),
      $param
    );

    if(!$param['pager']){
      unset($param['items_per_page'], $param['page']);
    }

    parent::__construct(
      $param['arch'],
      new \Gulei\Provider\Arch($param['arch']),
      new \Oerp\Provider\TreeRecords($param),
      $param
    );

    $this->mods['hd']['raw'] = array(new \Oerp\Modifier\PackHeaderRaw());
    $this->mods['row']['raw'] = array(new \Oerp\Modifier\PackRowRaw());
  }

  protected function getFuncPrefix()
  {
    return array('GuFunc_', 'OerpFunc_');
  }

  protected function getDefaultParam()
  {
    return array(
      'mark_translated' => true,
      'mark_invalid' => true,
      'mark_notice' => true,
      'mark_missing' => true,
      'copy_method' => 'ditto',
      'items_per_page' => 80,
      'sticky_header' => true,
      'mark_translated' => false,
    );
  }

  protected function getCaption()
  {
    if($cap = $this->getArch()->getAttribute('string')){
      return sprintf('<div class="caption">%s</div>', $cap);
    }
    return '';
  }

  public function theme($hds = null, $rows = null, $param = null)
  {
    $inc = new \Oerp\Includer\View();
    $inc->includeCss();
    $inc->includeJs();
    return parent::theme($hds, $rows, $param);
  }
}