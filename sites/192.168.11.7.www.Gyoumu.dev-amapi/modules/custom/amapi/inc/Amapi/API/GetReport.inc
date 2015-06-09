<?php
namespace Amapi\API;

class GetReport extends Abstract_Call
{
  public function getResponse($args = null)
  {
    $parameters = array (
      'Merchant' => self::MERCHANT_ID,
      'Report' => @fopen('php://memory', 'rw+'),
      'ReportId' => $args[0]
    );

    $request = new \MarketplaceWebService_Model_GetReportRequest($parameters);
    $service = $this->getService();
    $service->getReport($request);

    $resp = stream_get_contents($request->getReport());
    $resp = iconv('SJIS', 'UTF-8', $resp);
    return $resp;
  }
}