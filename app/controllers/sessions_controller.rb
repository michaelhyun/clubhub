class SessionsController < ActionController::Base
layout "users"
skip_before_action :authorize
  def new
  end

	def create
		user = User.authenticate(params[:session][:email], params[:session][:password])
		if user.nil?
			flash.now[:error] = "Invalid email/password combination."
			render :new
		else
			login user
			redirect_to user
	end

  def destroy
    log_out
    redirect_to root_url
  end
end

