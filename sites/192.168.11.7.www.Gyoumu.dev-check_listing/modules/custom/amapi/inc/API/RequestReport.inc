<?php
namespace Amapi\API;

class RequestReport extends Abstract_Call
{
  public function getResponse($args = null)
  {
    $parameters = array (
      'Merchant' => self::MERCHANT_ID,
      'MarketplaceIdList' => array("Id" => array(self::MARKET_ID)),
      'ReportType' => $args[0],
    );

    if($args[1]){
      $parameters['ReportOptions'] = $args[1];
    }

    $request = new \MarketplaceWebService_Model_RequestReportRequest($parameters);
    $service = $this->getService();
    return $service->requestReport($request);
  }
}