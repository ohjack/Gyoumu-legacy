// Generated by CoffeeScript 1.7.1
(function() {
  var LoadSku,
    __bind = function(fn, me){ return function(){ return fn.apply(me, arguments); }; };

  $(document).ready(function() {
    var $rs, load_sku;
    load_sku = new LoadSku();
    $('#edit-load-sku-search-sku').click(function() {
      return load_sku.onClick('SearchSku');
    });
    $('#edit-load-sku-search-pmgr').click(function() {
      return load_sku.onClick('SearchPmgr');
    });
    $rs = $('#OutputStatistics-result');
    if ($rs.length) {
      return $(window).scrollTop($rs.position().top);
    }
  });

  LoadSku = (function() {
    function LoadSku() {
      this.onClick = __bind(this.onClick, this);
      this.getSend = __bind(this.getSend, this);
      this.sendPost = __bind(this.sendPost, this);
      this.getLoadSkuEle = __bind(this.getLoadSkuEle, this);
      this.setSkuInputText = __bind(this.setSkuInputText, this);
      this.getSkuInputEle = __bind(this.getSkuInputEle, this);
    }

    LoadSku.prototype.getSkuInputEle = function() {
      return $('#edit-skus');
    };

    LoadSku.prototype.setSkuInputText = function(text) {
      var $ele;
      $ele = this.getSkuInputEle();
      if (text === false) {
        return $ele.text('Loading...').attr('disabled', 'disabled');
      } else {
        return $ele.text(text).removeAttr('disabled');
      }
    };

    LoadSku.prototype.getLoadSkuEle = function() {
      return $('#edit-load-sku');
    };

    LoadSku.prototype.sendPost = function(send, func) {
      return $.post('?q=oerp/query/js', {
        send: send
      }, func);
    };

    LoadSku.prototype.getSend = function(type) {
      var key, tpl;
      key = this.getLoadSkuEle().val();
      tpl = ['product.product', [['state', '<>', 'obsolete']], ['default_code']];
      switch (type) {
        case 'SearchSku':
          tpl[1].push(['default_code', 'ilike', key]);
          break;
        case 'SearchPmgr':
          tpl[1].push(['product_manager', 'ilike', key]);
      }
      return tpl;
    };

    LoadSku.prototype.onClick = function(type) {
      var func, send;
      this.setSkuInputText(false);
      send = JSON.stringify(this.getSend(type));
      func = (function(_this) {
        return function(data) {
          var rec, resp, skus, _i, _len;
          resp = JSON.parse(data);
          if (resp != null) {
            skus = '';
            for (_i = 0, _len = resp.length; _i < _len; _i++) {
              rec = resp[_i];
              skus += rec.default_code + "\n";
            }
            return _this.setSkuInputText(skus);
          } else {
            return _this.setSkuInputText('');
          }
        };
      })(this);
      this.sendPost(send, func);
      return false;
    };

    return LoadSku;

  })();

}).call(this);