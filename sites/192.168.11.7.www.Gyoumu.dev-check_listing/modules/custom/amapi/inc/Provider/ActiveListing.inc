<?php
namespace Amapi\Provider;
use Amapi as A;

class ActiveListing implements \Gulei\InterfaceProvider
{
  const REPORT_TYPE = '_GET_MERCHANT_LISTINGS_DATA_';

  private $AliveTime;
  private $ts;
  private $data;

  public function __construct()
  {
    $this->AliveTime = new \DateTime('now -15min');
    $this->AliveTime->setTimezone(new \DateTimeZone('UTC'));
    $this->ts = time();
  }

  private function submitReportRequest()
  {
    $RequestReport = new A\API\RequestReport();
    return $RequestReport->getResponse(array(self::REPORT_TYPE, 'ShowSalesChannel=true'));
  }

  private function isExpired($time)
  {
    $avail_time = new \DateTime($time, new \DateTimeZone('UTC'));
    return strcmp($this->AliveTime->format('c'), $avail_time->format('c')) >= 0
        ? true : false;
  }

  private function hasRecentRequest()
  {
    $GetReportRequestList = new A\API\GetReportRequestList();
    $list = $GetReportRequestList->getResponse();

    foreach($list as $item){
      if( $item['ReportType'] == self::REPORT_TYPE
        && !$this->isExpired($item['SubmittedDate']->format('c')) ) {

        return true;
      }
    }
    return false;
  }

  private function getLatestReport()
  {
    $GetReportList = new A\API\GetReportList();
    $reports = $GetReportList->getResponse();

    if(!is_array($reports)){
      return false;
    }

    foreach($reports as $report){
      if($report['ReportType'] == self::REPORT_TYPE){
        if($this->isExpired($report['AvailableDate']) && !$this->hasRecentRequest()){
          $this->submitReportRequest();
        }
        return $report;
      }
    }
    return false;
  }

  private function getCacheId()
  {
    return __CLASS__;
  }

  private function getCache()
  {
    $cache = cache_get($this->getCacheId(), 'cache');

    if(!$cache || $cache->expire < $this->ts){
      return false;
    }
    return $cache->data;
  }

  private function setCache()
  {
    $expire = $this->ts + 60 * 15;
    cache_set($this->getCacheId(), $this->data, 'cache', $expire);
  }

  public function getData()
  {
    if(!$report = $this->getLatestReport()){
      return false;
    }

    if($cache = $this->getCache()){
      return $this->data = $cache;
    }

    $GetReport = new A\API\GetReport();
    $csv_src = $GetReport->getResponse(array('813320865'));

    $csv = new \parseCSV();
    $csv->auto($csv_src);
    $this->data = $csv->data;

    $this->setCache();
    return $this->data;
  }

  public function count()
  {
    return count($this->data);
  }

  public function getReference()
  {
    return null;
  }
}