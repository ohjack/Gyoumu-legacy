<?php
namespace Ebapi\Account;

class Helper
{
  const STATUS_NO_REC = 0;
  const STATUS_NEW = 1;
  const STATUS_CONSENTED = 2;

  const TYPE_PRODUCTION = 1;
  const TYPE_SANDBOX = 2;

  private $account;

  public function __construct($account)
  {
    $this->account = $account;
  }

  public static function getAccounts()
  {
    $sql = 'SELECT `account`, `type` FROM {ebapi_account} ORDER BY `type`, `account`';
    $rs = \db_query($sql);

    $type_map = array(
      self::TYPE_PRODUCTION => 'Production',
      self::TYPE_SANDBOX => 'Sandbox',
    );

    $recs = array();
    while($data = \db_fetch_object($rs)){
      $rec = array(
        'type' => $type_map[$data->type],
        'account' => $data->account
      );
      $recs[] = $rec;
    }
    return $recs;
  }

  public static function getListSource()
  {
    $recs = self::getAccounts();

    $arch = <<<XML
<tree>
  <field name="type" string="Type"/>
  <field name="account" string="Account"/>
</tree>
XML;

    $t = new \Gulei\Table\Arch($arch, new \Gulei\Provider\Lambda($recs));
    return $t->theme();
  }

  public function getRecord()
  {
    $sql = 'SELECT * FROM {ebapi_account} WHERE account="%s" LIMIT 1';
    $rs = \db_query($sql, $this->account);
    return \db_fetch_object($rs);
  }

  public function getStatus()
  {
    $status = $this->getRecord()->status;
    return $status ? $status : self::STATUS_NO_REC;
  }

  public function updateSessionID($sid)
  {
    $Account = new \stdClass();
    $Account->account = $this->account;
    $Account->session_id = $sid;
    $status = $this->getStatus();

    if($status){
      $resp = drupal_write_record('ebapi_account', $Account, 'account');
    }
    else{
      $Account->status = self::STATUS_NEW;
      $resp = drupal_write_record('ebapi_account', $Account);
    }
    return $resp;
  }

  public function updateUserToken($user_token, $accnt_type)
  {
    $Account = new \stdClass();
    $Account->account = $this->account;
    $Account->user_token = $user_token;
    $Account->type = $accnt_type;
    $status = $this->getStatus();

    if($status){
      $resp = drupal_write_record('ebapi_account', $Account, 'account');
    }
    else{
      $Account->status = self::STATUS_NEW;
      $resp = drupal_write_record('ebapi_account', $Account);
    }
    return $resp;
  }

  public function isSandbox()
  {
    $rec = $this->getRecord();
    return $rec->type == self::TYPE_SANDBOX;
  }

  public function getSession()
  {
    $helper = new \Ebapi\API\Helper($this->isSandbox());
    $sess = $helper->getSession();
    $sess->setToken($this->getRecord()->user_token);
    return $sess;
  }

  public function getQuery()
  {
    $sess = $this->getSession();
    return new \Services_Ebay($sess);
  }

  public function getItem($ItemID)
  {
    $Item = \Services_Ebay::loadModel('Item', $ItemID, $this->getSession());
    $Item->Get();
    return $Item;
  }
}