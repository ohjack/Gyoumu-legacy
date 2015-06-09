<?php
namespace Amapi\API;

class SubmitFeed extends \Amapi\API\Abstract_Call {
  protected function getFeedContent($args) {
    switch ($args['FeedType']) {
      case '_POST_PRODUCT_IMAGE_DATA_':
        $MessageType = 'ProductImage';
    }

    $txt = <<<TXT
<?xml version="1.0" encoding="utf-8" ?>
<AmazonEnvelope xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xsi:noNamespaceSchemaLocation="amzn-envelope.xsd">
  <Header>
    <DocumentVersion>1.01</DocumentVersion>
    <MerchantIdentifier>%s</MerchantIdentifier>
  </Header>

  <MessageType>%s</MessageType>
  %s
</AmazonEnvelope>
TXT;

    return sprintf(
      $txt, $args['Merchant'], $MessageType, $args['Messages']);
  }

  public function getResponse($args = NULL) {
    if (is_null($args)) {
      $args = array();
    }

    $args = array_merge(
      array (
        'Merchant' => self::MERCHANT_ID,
        'PurgeAndReplace' => FALSE,
      ),
      $args
    );
    /*ZA-04738*/
    $args['FeedContent'] = $this->getFeedContent($args);
    unset($args['Messages']);
    dpr($args);
    $feedHandle = @fopen('php://temp', 'rw+');
    fwrite($feedHandle, $args['FeedContent']);
    rewind($feedHandle);

    $args['FeedContent'] = $feedHandle;
    $args['ContentMd5'] = base64_encode(md5(stream_get_contents($feedHandle), true));
    rewind($feedHandle);

    $request = new \MarketplaceWebService_Model_SubmitFeedRequest($args);
    $service = $this->getService();
    return $service->submitFeed($request);
  }
}