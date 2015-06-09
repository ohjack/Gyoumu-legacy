<?php
namespace Amapi\API;

class GetFeedSubmissionResult extends \Amapi\API\Abstract_Call {
  public function getResponse($args = NULL) {
    $handle = @fopen('php://memory', 'rw+');

    $parameters = array (
      'Merchant' => self::MERCHANT_ID,
      'FeedSubmissionId' => '317899017',
      'FeedSubmissionResult' => $handle,
    );

    $request = new \MarketplaceWebService_Model_GetFeedSubmissionResultRequest($parameters);
    $service = $this->getService();
    $service->getFeedSubmissionResult($request);

    rewind($handle);
    return stream_get_contents($handle);
  }
}