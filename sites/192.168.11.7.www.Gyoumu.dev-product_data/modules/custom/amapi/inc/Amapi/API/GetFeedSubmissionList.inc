<?php
namespace Amapi\API;

class GetFeedSubmissionList extends \Amapi\API\Abstract_Call {
  public function getResponse($args = null) {
    $pars = array(
      'Merchant' => self::MERCHANT_ID
    );

    $request = new \MarketplaceWebService_Model_GetFeedSubmissionListRequest($pars);
    $service = $this->getService();
    return $service->getFeedSubmissionList($request);
  }
}