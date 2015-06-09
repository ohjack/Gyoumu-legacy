<?php
namespace Yauc\Lister\Provider;

abstract class _Abstract implements \Gulei\Provider\_Interface
{
  protected $sellerId;
  protected $status;
  protected $id;

  const Q_NEW = 1;
  const Q_DONE = 2;
  const Q_FAILED = 3;
  const Q_CANCEL = 4;

  const TABLENAME = 'yaucapi_lister_queue';

  public function getReference()
  {
    return array(
      'sellerId' => $this->sellerId,
      'status' => $this->status,
      'rec_id' => $this->id
    );
  }

  private function getWhereSellerId()
  {
    return is_null($this->sellerId)
        ? '' : sprintf('sellerId = "%s"', $this->sellerId);
  }

  private function getWhereStatus()
  {
    return is_null($this->status)
        ? '' : sprintf('status IN (%s)', implode($this->status, ','));
  }

  private function getWhereRecId()
  {
    return is_null($this->id)
        ? '' : 'id = ' . $this->id;
  }

  private function getSqlWhere()
  {
    $cri = array(
      $this->getWhereSellerId(),
      $this->getWhereStatus(),
      $this->getWhereRecId()
    );

    $cri = array_filter($cri);

    if(empty($cri)){
      return '';
    }
    else{
      return 'WHERE ' . implode(' AND ', $cri);
    }
  }

  protected function getSqlCount()
  {
    $sql = 'SELECT COUNT(*) FROM {' . self::TABLENAME . '} %s';
    return sprintf($sql, $this->getSqlWhere());
  }

  protected function getSqlFetch()
  {
    $sql = 'SELECT * FROM {yaucapi_lister_queue} %s';
    return sprintf($sql, $this->getSqlWhere());
  }

  protected function prepareRecord($rec)
  {
    $rec = array_merge($rec, (array)json_decode($rec['src']));
    unset($rec['src']);

    $gallery = $rec['gallery'];

    if(strpos($gallery, '/home/gulei/SmbGallery/') === 0){
      $rec['pic_path'] = $gallery;
    }
    else{
      preg_match("/.*(\\\\Publish\\\\.*)/", $gallery, $m);
      $path = '/home/gulei/SmbGallery' . str_replace("\\", '/', $m[1]);
      $rec['pic_path'] = $path;
    }

    $ptrs = array();
    foreach($rec as $fld_name => $val){
      if(preg_match('/^(.*)_lv(\d)$/', $fld_name, $m)){
        $ptrs[$m[2]][$m[1]] = $val;
      }
    }
    $rec['category'] = $ptrs;
    return $rec;
  }

  public function getData()
  {
    $sql = $this->getSqlFetch();
    $rs = db_query($sql, $this->sellerId);

    $recs = array();
    while($data = db_fetch_array($rs)){
      $recs[] = $this->prepareRecord($data);
    }
    return $recs;
  }
}