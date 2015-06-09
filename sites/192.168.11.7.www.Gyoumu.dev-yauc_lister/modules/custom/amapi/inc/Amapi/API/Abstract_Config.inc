<?php
namespace Amapi\API;

abstract class Abstract_Config
{
  const AWS_ACCESS_KEY_ID = 'AKIAIFSWXNUB4QUYVTFA';
  const AWS_SECRET_ACCESS_KEY = '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';
  const APPLICATION_NAME = 'amapi';
  const APPLICATION_VERSION = '2009-01-01';
  const MERCHANT_ID = 'A3QQ4Y336GHS7W';
  const MARKET_ID = 'A1VC38T7YXB528';
  const DATE_FORMAT = 'Y-m-d\TH:i:s\Z';

  private $serviceUrl;

  public function __construct()
  {
    $this->serviceUrl = "https://mws.amazonservices.jp";
  }

  protected function getService()
  {
    $config = array (
      'ServiceURL' => $this->serviceUrl,
      'ProxyHost' => null,
      'ProxyPort' => -1,
      'MaxErrorRetry' => 3,
    );

    return new \MarketplaceWebService_Client(
      self::AWS_ACCESS_KEY_ID,
      self::AWS_SECRET_ACCESS_KEY,
      $config,
      self::APPLICATION_NAME,
      self::APPLICATION_VERSION
    );
  }
}