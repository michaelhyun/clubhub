class SessionsController < ActionController::Base
layout "users"
skip_before_action :authorize


  def new
  end



	def create
    user = User.find_by(email: params[:session][:email].downcase)
   if user && user.authenticate(params[:session][:password])
    session[:user_id] = user.id
    redirect_to user
		else
      render 'new'
	  end

  end


  def destroy
    session.delete(:user_id)
    @current_user = nil
    redirect_to root_url
  end

end
