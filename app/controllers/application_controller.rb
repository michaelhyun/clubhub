class ApplicationController < ActionController::Base
  layout "users"
  
  protect_from_forgery with: :exception
  include SessionsHelper
end
