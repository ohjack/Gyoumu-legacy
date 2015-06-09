<?php
namespace Amapi\API;

class GetReportRequestList extends Abstract_Call
{
  public function getResponse($args = null)
  {
    $parameters = array (
      'Merchant' => self::MERCHANT_ID,
    );
    $request = new \MarketplaceWebService_Model_GetReportRequestListRequest($parameters);
    $service = $this->getService();
    $rs = $service->getReportRequestList($request);

    $list = $rs->getGetReportRequestListResult();

    $list_send = array();
    foreach($list->getReportRequestInfoList() as $each){
      $list_send[] = array(
        'ReportRequestId' => $each->getReportRequestId(),
        'ReportType' => $each->getReportType(),
        'StartDate' => $each->getStartDate(),
        'EndDate' => $each->getEndDate(),
        'Scheduled' => $each->getScheduled(),
        'SubmittedDate' => $each->getSubmittedDate(),
        'ReportProcessingStatus' => $each->getReportProcessingStatus(),
      );
    }
    return $list_send;
  }
}