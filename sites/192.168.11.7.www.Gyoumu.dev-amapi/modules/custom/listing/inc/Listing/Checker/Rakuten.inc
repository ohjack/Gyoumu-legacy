<?php
namespace Listing\Checker;

class Rakuten extends _Abstract
{
  const SLEEP_NANO = 500000000;
  const SEARCH_PAT = 'http://search.rakuten.co.jp/search/inshop-mall/%s/-/sid.264316-st.A';
  const PAGER_PAT = 'http://search.rakuten.co.jp/search/inshop-mall/%s/-/f.1-p.%s-s.1-sf.0-sid.264316-st.A-v.2';
  const ITEM_LINK_REGEX_PAT = '@http://item\.rakuten\.co\.jp/titosoy/[^/\'"]*@';

  static private $item_links = array();

  public function getKey()
  {
    return __CLASS__;
  }

  public function getHeader()
  {
    return 'Rakuten';
  }

  private function sleep()
  {
    time_nanosleep(0, self::SLEEP_NANO);
  }

  private function getSearchResult($key)
  {
    $resp = file_get_contents(sprintf(self::SEARCH_PAT, $key));
    $result = $resp;
    $page = 2;

    while(preg_match('/次の\d*件/Uu', $resp)){
      $this->sleep();
      $resp = file_get_contents(sprintf(self::PAGER_PAT, $key, $page));
      $result .= $resp;
      $page++;
    }
    return $result;
  }

  private function findItems()
  {
    $ArryObj = new \Gulei\_Array\Basic($this->getSkus());
    $keys = $ArryObj->exe('Listing\_Array\Command\Amazon\getSearchKeywords');

    foreach($keys as $key){
      $resp = $this->getSearchResult($key);

      preg_match_all(self::ITEM_LINK_REGEX_PAT, $resp, $m);
      self::$item_links = array_merge(self::$item_links, $m[0]);
      self::$item_links = array_values(array_unique(self::$item_links));

      $this->sleep();
    }
  }

  protected function findItem($sku)
  {
    if(!self::$item_links){
      $this->findItems();
    }

    $links = array();
    foreach(self::$item_links as $item_link){
      $pat = sprintf('@/%s(-.*)?$@', strtolower($sku));
      if(preg_match($pat, $item_link)){
        $links[] = $item_link;
      }
    }
    return $links;
  }

  protected function getLinkOpt()
  {
    $opt = parent::getLinkOpt();
//    $opt['attributes']['class'] = 'check-online';
    return $opt;
  }

  protected function getLink($sku)
  {
    $items = $this->findItem($sku);
    return $items;
  }

  public function checkOnline($href)
  {
    $resp = iconv('EUC-JP', 'UTF-8', file_get_contents($href));

    if(strpos($resp, '買い物かごに入れる') !== false){
      return array(
        'success' => true,
        'href' => $href,
      );
    }
    else{
      return array(
        'success' => false,
        'href' => $href,
        'msg' => $this->getErrorMsg()
      );
    }
  }
}