<?php
namespace Yauc\Lister\Page;

class SellersView extends \Yauc\Page\SellersView
{
  protected function getActionLinks($seller)
  {
    $links = parent::getActionLinks($seller);
    $links['queue'] = array(
      l('Queue', 'yaucapi/seller/' . $seller->seller_id . '/auctions/queue'),
      l('Import', 'yaucapi/seller/' . $seller->seller_id . '/auctions/queue/import'),
    );
    return $links;
  }
}
