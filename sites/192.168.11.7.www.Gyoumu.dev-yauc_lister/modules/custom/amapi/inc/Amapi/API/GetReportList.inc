<?php
namespace Amapi\API;

class GetReportList extends Abstract_Call
{
  private function getList($resp)
  {
    $rs = $resp->getGetReportListResult();

    $list = array();
    foreach($rs->getReportInfoList() as $report){
      $item = array(
        'ReportId' => $report->getReportId(),
        'ReportRequestId' => $report->getReportRequestId(),
        'ReportType' => $report->getReportType(),
        'AvailableDate' => $report->getAvailableDate()->format(self::DATE_FORMAT),
      );
      $list[] = $item;
    }
    return $list;
  }

  public function getResponse($args = null)
  {
    $parameters = array (
      'Merchant' => self::MERCHANT_ID,
      'AvailableToDate' => new \DateTime('now', new \DateTimeZone('UTC')),
      'AvailableFromDate' => new \DateTime('-6 months', new \DateTimeZone('UTC')),
      'Acknowledged' => false,
    );

    $request = new \MarketplaceWebService_Model_GetReportListRequest($parameters);
    $service = $this->getService();
    $resp = $service->getReportList($request);
    return $this->getList($resp);
  }
}