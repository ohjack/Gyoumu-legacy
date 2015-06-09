<?php
namespace Oerp\Query;

class Arch extends \Oerp\Arch\Element
{
  protected $q;
  protected $offset = 0;
  protected $limit = 0;
  protected $pager = false;

  private function initPagerParam($params, $override)
  {
    $items = $params['items_per_page'];
    $items = ($over = $override['items_per_page']) ? $over : $items;
    $page = $params['page'] ? $params['page'] : $_GET['page'];
    $pager = $override['pager'] ? true : $params['pager'] ? true : false;

    $this->limit = $items;
    $this->offset = $items * $page;
    $this->pager = $pager;
  }

  private function initFieldStrings()
  {
    $defs = \Oerp\Query\Fields::getDef($this->getParam('oerp:model'));

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

    $this->q = new \Oerp\Query\Basic(
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