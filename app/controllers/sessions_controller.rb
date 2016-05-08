class SessionsController < ApplicationController::Base
	layout "users"
	skip_before_action :authorize
  def new
      @session = Session.new
  end
    def index
        if params[:status] == "activated"
            @session = Session.activated
        else
            @session = Session.unactivated
        end
    end
    def create
        @session = Session.new(params[:session])
        if @session.save
            redirect_to @session
        else
            render :action => "new"
        end
    end
end

#  def create
#  	user = User.find_by(name: params[:name])
#  	if user and user.authenticate(params[:password])
#  		sessions[:user_id] = user.id
#  		redirect_to admin_url
#  	else
#  		redirect_to login_url , alert:"Invalid username or password"
#  	end
#  end
#
#  def destroy
#  	sessions[:user_id] = nil
#  	redirect_to login_url , alert:"Successfully logged out"
#  end
#end
